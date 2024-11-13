ex(nodeType *p) {
    switch(p->type) {
    case typeCon:       
        printf("\tpush\t%d\n", p->con.value); 
        break;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:
            printf("L%03d:\n", lbl1 = lbl++);
            ex(p->opr.op[0]);
            printf("\tjz\tL%03d\n", lbl2 = lbl++);
            ex(p->opr.op[1]);
            printf("\tjmp\tL%03d\n", lbl1);
            printf("L%03d:\n", lbl2);
            break;
        case IF:
            ex(p->opr.op[0]);
            printf("\tjz\tL%03d\n", lbl1 = lbl++);
            ex(p->opr.op[1]);
            printf("\tjmp\tL%03d\n", lbl2 = lbl++);
            printf("L%03d:\n", lbl1);
            ex(p->opr.op[2]);
            printf("L%03d:\n", lbl2);
            break;
        default:
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            switch(p->opr.oper) {
            case '+':   printf("\tadd\n"); break;
            case '-':   printf("\tsub\n"); break; 
            case '*':   printf("\tmul\n"); break;
            case '/':   printf("\tdiv\n"); break;
            }
        }
    }
}

     if(E) E else E          		    while(E) E
     ------------             		    ------------
     cod pentru E //Cond E    		L1: cod pentru E //Cond E
     JPC L1                       	    JPC L2
     cod pentru E //Then E		    cod pentru E //Body E
     JMP L2                      	    JMP L1
L1:  cod pentru E //Else E            	L2: ...
L2:  ...