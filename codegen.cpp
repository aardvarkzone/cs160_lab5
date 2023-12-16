#include <cassert>
#include <typeinfo>

#include "ast.hpp"
#include "symtab.hpp"
#include "primitive.hpp"
#include <iostream>
using namespace std;

class Codegen : public Visitor
{
  private:
    FILE* m_outputfile;
    SymTab *m_st;

    // Basic size of a word (integers and booleans) in bytes
    static const int wordsize = 4;

    int label_count; // Access with new_label
    // int size_of_locals;
    // Helpers
    // This is used to get new unique labels (cleverly names label1, label2, ...)
    int new_label()
    {
        return label_count++;
    }

    void set_text_mode()
    {
        fprintf(m_outputfile, ".text\n\n");
    }

    void set_data_mode()
    {
        fprintf(m_outputfile, ".data\n\n");
    }

    // PART 1:
    // 1) get arithmetic expressions on integers working:
    //  you wont really be able to run your code,
    //  but you can visually inspect it to see that the correct
    //  chains of opcodes are being generated.
    // 2) get procedure calls working:
    //  if you want to see at least a very simple program compile
    //  and link successfully against gcc-produced code, you
    //  need to get at least this far
    // 3) get boolean operation working
    //  before we can implement any of the conditional control flow
    //  stuff, we need to have booleans worked out.
    // 4) control flow:
    //  we need a way to have if-elses and while loops in our language.
    // 5) arrays: just like variables, but with an index

    // Hint: the symbol table has been augmented to track an offset
    //  with all of the symbols.  That offset can be used to figure
    //  out where in the activation record you should look for a particuar
    //  variable


    ///////////////////////////////////////////////////////////////////////////////
    //
    //  function_prologue
    //  function_epilogue
    //
    //  Together these two functions implement the callee-side of the calling
    //  convention.  A stack frame has the following layout:
    //
    //                         <- SP (before pre-call / after epilogue)
    //  high -----------------
    //       | actual arg 1  |
    //       |    ...        |
    //       | actual arg n  |
    //       -----------------
    //       |  Return Addr  |
    //       =================
    //       | temporary 1   | <- SP (when starting prologue)
    //       |    ...        |
    //       | temporary n   |
    //   low ----------------- <- SP (when done prologue)
    //
    //
    //              ||
    //              ||
    //             \  /
    //              \/
    //
    //
    //  The caller is responsible for placing the actual arguments
    //  and the return address on the stack. Actually, the return address
    //  is put automatically on the stack as part of the x86 call instruction.
    //
    //  On function entry, the callee
    //
    //  (1) allocates space for the callee's temporaries on the stack
    //
    //  (2) saves callee-saved registers (see below) - including the previous activation record pointer (%ebp)
    //
    //  (3) makes the activation record pointer (frmae pointer - %ebp) point to the start of the temporary region
    //
    //  (4) possibly copies the actual arguments into the temporary variables to allow easier access
    //
    //  On function exit, the callee:
    //
    //  (1) pops the callee's activation record (temporay area) off the stack
    //
    //  (2) restores the callee-saved registers, including the activation record of the caller (%ebp)
    //
    //  (3) jumps to the return address (using the x86 "ret" instruction, this automatically pops the
    //      return address off the stack
    //
    //////////////////////////////////////////////////////////////////////////////
    //
    // Since we are interfacing with code produced by GCC, we have to respect the
    // calling convention that GCC demands:
    //
    // Contract between caller and callee on x86:
    //    * after call instruction:
    //           o %eip points at first instruction of function
    //           o %esp+4 points at first argument
    //           o %esp points at return address
    //    * after ret instruction:
    //           o %eip contains return address
    //           o %esp points at arguments pushed by caller
    //           o called function may have trashed arguments
    //           o %eax contains return value (or trash if function is void)
    //           o %ecx, %edx may be trashed
    //           o %ebp, %ebx, %esi, %edi must contain contents from time of call
    //    * Terminology:
    //           o %eax, %ecx, %edx are "caller save" registers
    //           o %ebp, %ebx, %esi, %edi are "callee save" registers
    ////////////////////////////////////////////////////////////////////////////////


    void emit_prologue(SymName *name, unsigned int size_locals, unsigned int num_args)
    {
        cout << "    # Prologue" << endl;
        cout << name->spelling() << ":" << endl;
        cout << "    pushl %ebp" << endl;
        cout << "    movl  %esp, %ebp" << endl;
        cout << "    subl  $" << size_locals << ", %esp" << endl;

        if (num_args > 0) {
            for (unsigned int i = 0; i < num_args; i++) {
                int offset = 8 + (i * 4);
                int temp_offset = -4 * (i + 1);
                cout << "    movl  " << offset << "(%ebp), %eax" << endl;  
                cout << "    movl  %eax, " << temp_offset << "(%ebp)" << endl;  
            }
        }
    }

    void emit_epilogue()
    {
        cout << "    # Epilogue" << endl;
        // cout << "    movl  %ebp, %esp" << endl;
        // cout << "    popl  %ebp" << endl;
        
        cout << "    ret" << endl;
    }

  // WRITEME: more functions to emit code

  public:

    // int calculate_size_of_locals(std::list<Decl_ptr>* decl_list) {
    //     int size = 0;
    //     for (auto it = decl_list->begin(); it != decl_list->end(); ++it) {
    //         // Assuming each variable takes 4 bytes
    //         size += 4;
    //     }
    //     return size;
    // }

    Codegen(FILE* outputfile, SymTab* st)
    {
        m_outputfile = outputfile;
        m_st = st;
        label_count = 0;
        // size_of_locals = 0;
        // cout << ".text\n" << endl;
        cout << ".globl Main" << endl;
    }

    void visitProgramImpl(ProgramImpl* p)
    {
        p->visit_children(this);
    }

    void visitProcImpl(ProcImpl* p)
    {
        list<Decl_ptr>::iterator it;
        int size_of_locals = 0;
        // cout << "Number of declarations: " << p->m_decl_list->size() << endl;
        
        for (it = p->m_decl_list->begin(); it != p->m_decl_list->end(); ++it) {
            Decl_ptr decl = *it;
            Basetype type = decl->m_attribute.m_basetype;
            int type_size = 0;
            switch(type) {
                case bt_string:
                    type_size = 4; 
                    break;
                case bt_undef: 
                    type_size = 0; 
                    break;
                default:
                    type_size = 4; 
            }
            size_of_locals += 4;
        }
        
        Procedure_blockImpl *procblockImpl = dynamic_cast<Procedure_blockImpl*>(p->m_procedure_block);
        std::list<Decl_ptr>::iterator it_2;
        int size = 0;
        for(it_2 = procblockImpl->m_decl_list->begin();
            it_2 != procblockImpl->m_decl_list->end();
            ++it_2){
                DeclImpl *decl_impl = dynamic_cast<DeclImpl*>(*it_2);
                list<SymName_ptr>::iterator m_symname_list_iter;

                for(m_symname_list_iter = decl_impl->m_symname_list->begin();
                        m_symname_list_iter != decl_impl->m_symname_list->end();
                        ++m_symname_list_iter)
                    {
                        SymName *s = dynamic_cast<SymName*>(*m_symname_list_iter);
                        if (m_st->lookup(p->m_attribute.m_scope, s->spelling())->get_size() == 4 || 
                             m_st->lookup(p->m_attribute.m_scope, s->spelling())->get_size() == 1) {
                            size += 4;
                        } else {
                            size += m_st->lookup(p->m_attribute.m_scope, s->spelling())->get_size();
                        }
                    }
        }
        size_of_locals += size;
        int num_args = m_st->lookup(p->m_symname->spelling())->m_arg_type.size(); 
        // cout << "\nnum_args: " << num_args << "\n" << endl;
       

        emit_prologue(p->m_symname, size_of_locals, num_args);
        // cout << "\n" << size_of_locals << "\n\n";

        p->visit_children(this);
        // size_of_locals = calculate_size_of_locals(p->m_decl_list);

        // int num_args = m_st->lookup(p->m_symname->spelling())->m_arg_type.size(); 

        // emit_prologue(p->m_symname, size_of_locals, num_args);

        // p->visit_children(this);
        // cout << "\n#size_of_locals after visit: " << size_of_locals << "\n\n";
        // cout << "\n#end of visitProcImpl\n\n";
    }

    void visitProcedure_blockImpl(Procedure_blockImpl* p)
    {
        // list<Decl_ptr>::iterator it;
        // for (it = p->m_decl_list->begin(); it != p->m_decl_list->end(); ++it) {
        //     Decl_ptr decl = *it;
        //     Basetype type = decl->m_attribute.m_basetype;
        //     int type_size = 0;
        //     switch(type) {
        //         case bt_string:
        //             type_size = 4; 
        //             break;
        //         case bt_undef: 
        //             type_size = 0; 
        //             break;
        //         default:
        //             type_size = 4; 
        //     }
        //     size_of_locals += 4;
        // }
        p->visit_children(this);
        // size_of_locals += calculate_size_of_locals(p->m_decl_list);

        // p->visit_children(this);
    }

    void visitNested_blockImpl(Nested_blockImpl* p)
    {
        p->visit_children(this);
    }

    void visitAssignment(Assignment* p)
    {
        cout << "    #visit Assignment\n";
        p->m_expr->accept(this);
        p->m_lhs->accept(this);
        // cout << "    popl %ebx" << endl;
        // cout << "    # pop var" << endl;
        // cout << "    popl %eax" << endl;
        // cout << "    # pop val" << endl;
        // cout << "    movl %eax, (%ebx)" << endl;
        // cout << "    # assign" << endl;
        cout << "    popl %eax #offset" << endl;
        cout << "    popl %ebx #value" << endl;
        cout << "    negl %eax #offset" << endl;
        cout << "    movl %ebx, (%ebp, %eax)" << endl;
        cout << "    # assign" << endl;
    }


    void visitCall(Call* p)
    {
        list<Expr_ptr>::iterator it;
        for (it = p->m_expr_list->begin(); it != p->m_expr_list->end(); ++it) {
            (*it)->accept(this);
        }
        cout << "    #visit Call" << endl;
        // p->visit_children(this); 

        cout << "    call " << p->m_symname->spelling() << endl;
        cout << "    pushl  %eax" << endl; 
        if (p->m_lhs) {
            p->m_lhs->accept(this);  
            cout << "    popl  %ebx" << endl; 
            cout << "    popl  %eax" << endl; 
            cout << "    negl  %ebx" << endl;
            cout << "    movl  %eax, (%ebp, %ebx)" << endl; 
        }
    }

    void visitReturn(Return* p)
    {
        cout << "    # visit Return" << endl;
        if (p->m_expr) {
            p->m_expr->accept(this);
            cout << "    popl  %eax # for return" << endl;  
        }

        cout << "    leave" << endl; 
        emit_epilogue();
    }

    // Control flow
    void visitIfNoElse(IfNoElse* p)
    {
        int label_end = new_label();

        p->m_expr->accept(this);

        cout << "    popl  %eax" << endl;
        cout << "    cmpl  $0, %eax" << endl;
        cout << "    je    label" << label_end << endl;

        p->m_nested_block->accept(this);
        cout << "label" << label_end << ":" << endl;
    }

    void visitIfWithElse(IfWithElse* p) {
        int label_else = new_label();
        int label_end = new_label();

        p->m_expr->accept(this); 

        cout << "    popl  %eax" << endl;
        cout << "    cmpl  $0, %eax" << endl;

        cout << "    je    label" << label_else << endl; //f

        p->m_nested_block_1->accept(this);

        cout << "    jmp   label" << label_end << endl;
        cout << "label" << label_else << ":" << endl;
        p->m_nested_block_2->accept(this);

        cout << "label" << label_end << ":" << endl;
    }


    void visitWhileLoop(WhileLoop* p)
    {
        int label_start = new_label();
        int label_end = new_label();

        cout << "label" << label_start << ":" << endl;

        p->m_expr->accept(this); 
        cout << "    popl  %eax" << endl; 
        cout << "    cmpl  $0, %eax" << endl;
        cout << "    je    label" << label_end << endl;

        p->m_nested_block->accept(this);

        cout << "    jmp   label" << label_start << endl;

        cout << "label" << label_end << ":" << endl;
        }

    void visitCodeBlock(CodeBlock *p) 
    {
        p->visit_children(this);
    }

    // Variable declarations (no code generation needed)
    void visitDeclImpl(DeclImpl* p)
    {
        cout << "    # visit VariableDecl" << endl;
        p->visit_children(this);
    }

    void visitTInteger(TInteger* p)
    {
        p->visit_children(this);
    }

    void visitTIntPtr(TIntPtr* p)
    {
        p->visit_children(this);
    }

    void visitTBoolean(TBoolean* p)
    {
        p->visit_children(this);
    }

    void visitTCharacter(TCharacter* p)
    {
        p->visit_children(this);
    }

    void visitTCharPtr(TCharPtr* p)
    {
        p->visit_children(this);
    }

    void visitTString(TString* p)
    {
        p->visit_children(this);
    }

    // Comparison operations
    void visitCompare(Compare* p)
    {
        int label_true = new_label();
        int label_end = new_label();

        p->visit_children(this);

        cout << "# Compiling ==" << endl; 
        cout << "    popl  %ebx" << endl;  
        cout << "    popl  %eax" << endl; 
        cout << "    cmpl  %ebx, %eax" << endl; 


        cout << "    je    label" << label_true << endl;

        cout << "    pushl $0" << endl;
        cout << "    jmp   label" << label_end << endl;

        cout << "label" << label_true << ":" << endl;
        cout << "    pushl $1" << endl;

        cout << "label" << label_end << ":" << endl;
    }

    void visitNoteq(Noteq* p) {
        int label_true = new_label();
        int label_end = new_label();

        p->visit_children(this);

        cout << "# Compiling !=" << endl; 
        cout << "    popl  %ebx" << endl; 
        cout << "    popl  %eax" << endl; 
        cout << "    cmpl  %ebx, %eax" << endl; 
        cout << "    jne   label" << label_true << endl; 

        cout << "    pushl $0" << endl;
        cout << "    jmp   label" << label_end << endl;

        cout << "label" << label_true << ":" << endl;
        cout << "    pushl $1" << endl;

        cout << "label" << label_end << ":" << endl;
    }

    void visitGt(Gt* p) {
        int label_true = new_label();
        int label_end = new_label();

        p->visit_children(this);

        cout << "# Compiling >" << endl;
        cout << "    popl  %ebx" << endl;
        cout << "    popl  %eax" << endl;
        cout << "    cmpl  %ebx, %eax" << endl;
        cout << "    jg    label" << label_true << endl;

        cout << "    pushl $0" << endl;
        cout << "    jmp   label" << label_end << endl;

        cout << "label" << label_true << ":" << endl;
        cout << "    pushl $1" << endl;

        cout << "label" << label_end << ":" << endl;
    }

    void visitGteq(Gteq* p)
    {
        int label_true = new_label();
        int label_end = new_label();

        p->visit_children(this);

        cout << "# Compiling >=" << endl;
        cout << "    popl  %ebx" << endl;
        cout << "    popl  %eax" << endl;
        cout << "    cmpl  %ebx, %eax" << endl;
        cout << "    jge   label" << label_true << endl;

        cout << "    pushl $0" << endl;
        cout << "    jmp   label" << label_end << endl;

        cout << "label" << label_true << ":" << endl;
        cout << "    pushl $1" << endl;

        cout << "label" << label_end << ":" << endl;
    }

    void visitLt(Lt* p)
    {
        int label_true = new_label();
        int label_end = new_label();

        p->visit_children(this);

        cout << "# Compiling <" << endl;
        cout << "    popl  %ebx" << endl;
        cout << "    popl  %eax" << endl;
        cout << "    cmpl  %ebx, %eax" << endl;
        cout << "    jl    label" << label_true << endl;

        cout << "    pushl $0" << endl;
        cout << "    jmp   label" << label_end << endl;

        cout << "label" << label_true << ":" << endl;
        cout << "    pushl $1" << endl;

        cout << "label" << label_end << ":" << endl;
    }

    void visitLteq(Lteq* p)
    {
        int label_true = new_label();
        int label_end = new_label();

        p->visit_children(this);

        cout << "# Compiling <=" << endl;
        cout << "    popl  %ebx" << endl;
        cout << "    popl  %eax" << endl;
        cout << "    cmpl  %ebx, %eax" << endl;
        cout << "    jle   label" << label_true << endl;

        cout << "    pushl $0" << endl;
        cout << "    jmp   label" << label_end << endl;

        cout << "label" << label_true << ":" << endl;
        cout << "    pushl $1" << endl;

        cout << "label" << label_end << ":" << endl;
    }

    // Arithmetic and logic operations
    void visitAnd(And* p)
    {
        p->visit_children(this); 
        cout << "# Compiling p" << endl; 
        cout << "    andl %ebx, %eax" << endl;
    }

    void visitOr(Or* p)
    {
        p->visit_children(this); 
        cout << "# Compiling p" << endl; 
        cout << "    orl %ebx, %eax" << endl;
    }

    void visitMinus(Minus* p)
    {
        cout << "# visit Minus" << endl; 
        if (p->m_expr_1->m_attribute.m_basetype = bt_charptr) {
            cout << "# charptr Minus" << endl; 
            cout << "    popl  %ebx" << endl; 
            cout << "    popl  %eax" << endl; 
            cout << "    negl  %ebx" << endl; 
            cout << "    imull $4, %ebx" << endl;
            cout << "    subl %ebx, %eax" << endl;
            cout << "    pushl %eax" << endl;
        } else {
            cout << "    popl  %ebx" << endl; 
            cout << "    popl  %eax" << endl; 
            cout << "    subl  %ebx, %eax" << endl; 
            cout << "    pushl  %eax" << endl;
        }
        
        
    }

    void visitPlus(Plus* p)
    {
        cout << "# visit Plus" << endl; 
        if (p->m_expr_1->m_attribute.m_basetype = bt_charptr) {
            cout << "# charptr Plus" << endl; 
            cout << "    popl  %ebx" << endl; 
            cout << "    popl  %eax" << endl; 
            cout << "    negl  %ebx" << endl; 
            cout << "    imull $4, %ebx" << endl;
            cout << "    addl %ebx, %eax" << endl;
            cout << "    pushl %eax" << endl;
        } else {
            cout << "    popl  %ebx" << endl; 
            cout << "    popl  %eax" << endl; 
            cout << "    addl  %ebx, %eax" << endl; 
            cout << "    pushl  %eax" << endl;
        }
        
    }

    void visitTimes(Times* p)
    {
        p->visit_children(this); 
        cout << "    # Compiling *" << endl; 
        cout << "    popl  %ebx" << endl; 
        cout << "    popl  %eax" << endl; 
        cout << "    imul  %ebx, %eax" << endl; 
        cout << "    pushl  %eax" << endl;
    }

    void visitDiv(Div* p)
    {
        p->visit_children(this); 
        cout << "    # Compiling /" << endl;
        cout << "    popl  %ebx" << endl; 
        cout << "    popl  %eax" << endl; 
        cout << "    cdq  %ebx, %eax" << endl; 
        cout << "    idiv  %ebx";
        cout << "    pushl  %eax" << endl;
    }

    void visitNot(Not* p)
    {
        p->visit_children(this); 
        cout << "    # Compiling !" << endl;
        cout << "    popl %eax" << endl;
        cout << "    xor %eax" << endl;
        cout << "    pushl %eax" << endl;
    }

    void visitUminus(Uminus* p) {
        p->visit_children(this);
        cout << "    popl %eax" << endl;
        cout << "    neg %eax" << endl;
        cout << "    pushl %eax" << endl;       
    }

    // Variable and constant access
    void visitIdent(Ident* p)
    {
        cout << "    #visit Ident" << endl;
        p->visit_children(this);
        const char* identName = p->m_symname->spelling();
        Symbol* sym = m_st->lookup(identName);
        if (sym != NULL) {
            int offset = sym->get_offset();
            cout << "    movl  " << -offset << "(%ebp), %eax" << endl;  
            cout << "    pushl %eax" << endl;  
        }   
    }

    void visitBoolLit(BoolLit* p)
    {
        cout << "    pushl $" << p->m_primitive->m_data << endl;
    }

    void visitCharLit(CharLit* p)
    {
        cout << "    pushl $" << p->m_primitive->m_data << endl;
    }

    void visitIntLit(IntLit* p)
    {
        cout << "    #visit IntLit" << endl;
        cout << "    pushl $" << p->m_primitive->m_data << "\n";
    }

    void visitNullLit(NullLit* p)
    {
        cout << "    #visit NullLit" << endl;
        cout << "    pushl $0" << endl;
    }

    void visitArrayAccess(ArrayAccess* p)
    {
        p->visit_children(this);

        cout << "    popl  %edx" << endl;  

        Symbol* sym = m_st->lookup(p->m_symname->spelling());
        if (sym != NULL) {
            int baseOffset = sym->get_offset();  

            cout << "    leal  " << baseOffset << "(%ebp), %eax" << endl;  
            cout << "    addl  %edx, %eax" << endl; 
            cout << "    movl  (%eax), %eax" << endl;

            cout << "    pushl %eax" << endl;
        }
    }

    // LHS


    void visitVariable(Variable* p)
    {
        cout << "    #visit Variable" << endl;
        if(p->m_attribute.m_basetype != bt_string) {
            p->visit_children(this);        
            int offset = 4 + m_st->lookup(p->m_attribute.m_scope, p->m_symname->spelling())->get_offset();
            cout << "    pushl $" << offset << " #offset from visitVar" << endl; 
        }
    }

    

    void visitDerefVariable(DerefVariable* p)
    {
        p->visit_children(this);
        int offset  = 4 + m_st->lookup(p->m_attribute.m_scope, p->m_symname->spelling())->get_offset();
        cout << "    popl %eax" << endl;
        cout << "    movl -" << offset <<  "(%ebp), %ebx" << endl;  

        cout << "    movl %eax (%ebx)" << endl;   
    }

    void visitArrayElement(ArrayElement* p)
    {
        p->visit_children(this);

        cout << "    popl  %edx" << endl;  

        Symbol* sym = m_st->lookup(p->m_symname->spelling());
        if (sym != NULL) {
            int baseOffset = sym->get_offset();  

            cout << "    leal  " << baseOffset << "(%ebp), %eax" << endl;  
            cout << "    addl  %edx, %eax" << endl;  

            cout << "    movl  (%eax), %eax" << endl;

            cout << "    pushl %eax" << endl;
        }
    }

    // Special cases
    void visitSymName(SymName* p)
    {
    }

    void visitPrimitive(Primitive* p)
    {
    }

    // Strings
    void visitStringAssignment(StringAssignment* p)
    {
        p->visit_children(this); 
        
        cout << "    popl  %esi" << endl; 
        cout << "    popl  %edi" << endl; 
        
        cout << "    movl  $0, %ecx" << endl;  // loop count
        cout << "string_copy_loop:" << endl;
        cout << "    movb  (%esi, %ecx, 1), %al" << endl;  
        cout << "    movb  %al, (%edi, %ecx, 1)" << endl;  
        cout << "    incl  %ecx" << endl;                  
        cout << "    testb %al, %al" << endl;             
        cout << "    jne   string_copy_loop" << endl; 
    }

    void visitStringPrimitive(StringPrimitive* p)
    {
        cout << "    pushl $" << p->m_string << endl;
    }

    void visitAbsoluteValue(AbsoluteValue* p)
    {
        int label_is_positive = new_label();
        int label_end = new_label();

        p->visit_children(this);

        cout << "    popl  %eax" << endl;

        cout << "    cmpl  $0, %eax" << endl;
        cout << "    jge   label" << label_is_positive << endl;

        cout << "    negl  %eax" << endl;

        cout << "    jmp   label" << label_end << endl;

        cout << "label" << label_is_positive << ":" << endl;

        cout << "label" << label_end << ":" << endl;

        cout << "    pushl %eax" << endl;
    }

    // Pointer
    void visitAddressOf(AddressOf* p)
    {  
        p->visit_children(this); 
        cout << "    popl %eax" << endl;
        cout << "    negl %eax" << endl;
        cout << "    leal (%ebp, %eax, 1), %eax" << endl;
        cout << "    popl %eax" << endl;

    }

    void visitDeref(Deref* p)
    {
        p->visit_children(this); 
    }
};


void dopass_codegen(Program_ptr ast, SymTab* st)
{
    Codegen* codegen = new Codegen(stdout, st);
    ast->accept(codegen);
    delete codegen;
}
