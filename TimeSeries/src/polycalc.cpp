#include "poly.h"
#include "NewQueues.h"
#include <stdlib.h>

#ifdef __GNUC__  //SUCKS
template class Queue<Polynomial>;
#endif

int ChkDiv(Polynomial *left, Polynomial *right, Polynomial *div, 
           PolynomialRatio *rat)
{
   Polynomial *temp;
   int chk;

   temp=Multiply(div,right);
   if (rat!=0) {
      temp->Add(rat->GetNumerator());
   }
   
   chk=left->Equals(temp);


   if (chk) { 
      printf("Incorrect division\n");
      left->Dump();
      printf("/\n");
      right->Dump();
      printf("=\n");
      div->Dump();
      if (rat) {
         printf("+\n");
         rat->Dump();
      }
      printf("BUT Multiplying out we get\n");
      temp->Dump();
   }

   delete temp;

   return chk;
}

void GetPoly(Polynomial *p, FILE *in)
{
   int power, numc, i;
   double coeff;

   scanf("%d",&power);
   p->SetPower(power);
   fscanf(in,"%d",&numc);
   for (i=0;i<numc;i++) {
      fscanf(in,"%lf",&coeff);
      p->SetCoeff(i,coeff);
   }
}


int main()
{
   Polynomial *left, *right;

   Stack<Polynomial> stack;

#define CHK_POP()\
            right = stack.Pop();  \
         if (right==0) { \
            printf("Stack Empty!\n"); \
            break;\
         }\
         left = stack.Pop(); \
         if (left==0) { \
            printf("Stack only has one poly!\n");\
            stack.Push(right);\
            break;\
         }\

   while (1) {
      fflush(stdin);
      printf("Enter command [n(power numc coeffs+)ps+-*/(t)^(p)d(d t)q]:");
      switch(getchar()) {
      case 'n': // New Poly:
          left = new Polynomial;
          GetPoly(left,stdin);
          stack.Push(left);
          break;
      case 'p': // pop and print poly
         left = stack.Pop();
         if (left==0) {
            printf("Stack is empty\n");
         } else {
            left->Dump();
         }
         delete left;
         break;
      case 's': // show stack
         printf("Stack Contents\n");
         for (left=stack.First();left!=0;left=stack.Next()) {
            left->Dump();
         }
         break;
      case '+':
         right = stack.Pop();
         CHK_POP()
         left->Add(right);
         left->Dump();
         delete right;
         stack.Push(left);
        break;
      case '-':
         CHK_POP();
         left->Subtract(right);
         left->Dump();
         delete right;
         stack.Push(left);
        break;
      case '*':
         CHK_POP();
         left->MultiplyBy(right);
         left->Dump();
         delete right;
         stack.Push(left);
        break;
      case '/': {
         int trunc;
         scanf("%d",&trunc);
         CHK_POP();
         Polynomial *res;
         PolynomialRatio *rem;
         res = Divide(left,right,&rem,trunc);
         res->Dump();
         rem->Dump();
         stack.Push(res);
         delete left;
         delete right;
         delete rem;
         break;
                }
      case '^': {
         int power;
         scanf("%d",&power);
         left=stack.Pop();
         if (left==0) { 
            printf("Nothing on stack!\n");
            break;
         }
         left->RaiseTo(power);
         left->Dump();
         stack.Push(left);
                }
         break;
      case 'd': {
	double d;
	int t;
	scanf("%lf",&d);
	scanf("%d",&t);
	Polynomial *poly=MakeDeltaFracD(d,t);
	stack.Push(poly);
      }
      break;
      case 'q': 
         exit(0);
         break;
      default:
         printf("Unknown Command\n");
         break;
      }
   }

   return 0;
}

 
