#include "poly.h"
#include "NewQueues.h"
#include <stdlib.h>
#include <ctype.h>
#include "banner.h"

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



void usage(const char *n) 
{
  char *b=GetRPSBanner();

  fprintf(stdout,
	  "RPN Polynomial Calculator\n\n%s\n\n%s\n\n",n,b);
  delete [] b;
}


int main(int argc, char *argv[])
{
   Polynomial *left, *right;

   Stack<Polynomial> stack;


   usage(argv[0]);

#define CHK_POP()\
            left = stack.Pop();  \
         if (left==0) { \
            printf("Stack Empty!\n"); \
            break;\
         }\
         right = stack.Pop(); \
         if (right==0) { \
            printf("Stack only has one poly!\n");\
            stack.Push(left);\
            break;\
         }\

   while (1) {
      char c;
      fflush(stdin);
      printf("Enter command [? for help]: ");
      while (!feof(stdin) && (c=getchar()) && isspace(c));
	;
      switch(c) {
      case 'n': // New Poly:
          left = new Polynomial;
          GetPoly(left,stdin);
          stack.Push(left);
          break;
      case 'c': // Clone
	  left = stack.Pop();
          if (left==0) {
            printf("Stack is empty\n");
	  } else {
            stack.Push(left);
	    stack.Push(new Polynomial(*left));
	  } 
          break;
      case 'x':
         CHK_POP()
         stack.Push(left);
	 stack.Push(right);
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
      case 't': 
	int num,i;
	scanf("%d",&num);
	for (i=0,left=stack.First();(left!=0) && (i<num); left=stack.Next(), i++)
	{}
	if (left) {
	  left->Dump();
	} else {
	  printf("Not That Many Items On Stack\n");
	}
	break;
      case 's': // show stack
         printf("Stack Contents\n");
         for (i=0,left=stack.First();left!=0;left=stack.Next(),i++) {
	   printf("%d:\t",i);
	   left->Dump();
         }
         break;
      case '+':
         CHK_POP()
         left->Add(right);
         delete right;
         stack.Push(left);
        break;
      case '-':
         CHK_POP();
         left->Subtract(right);
         delete right;
         stack.Push(left);
        break;
      case '*':
         CHK_POP();
         left->MultiplyBy(right);
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
	 stack.Push(res);
	 rem->Dump();
         delete left;
         delete right;
         delete rem;
                }
         break;
      case '^': {
         int power;
         scanf("%d",&power);
         left=stack.Pop();
         if (left==0) { 
            printf("Nothing on stack!\n");
            break;
         }
         left->RaiseTo(power);
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
      case '?' :
	fprintf(stdout,"\n"
		"?                     this help screen\n"
		"n pow numc coeffs+    push new polynomial of \n"
                "                       power pow with numc coefficients\n"
		"d delta trunc         push new fractional difference \n"
		"                       according to delta and truncated\n"
		"                       at trunc coefficients\n"
		"c                     clone the top of stack\n"
		"x                     swap top of stack and second\n"
		"p                     pop and print\n"
                "s                     show stack\n"
		"t num                 show stack entry num\n"
                "+                     add top two polys in stack \n"
		"                       and replace with sum\n"
		"-                     subtract second in stack from \n"
		"                       first and replace\n"
		"*                     multiply top two polys in stack \n"
		"                       and replace with product\n"
		"/ trunc               divide second in stack into \n"
		"                       first and replace with result\n"
		"                       computed to trunc places. The remainder\n"
		"                       ratio is printed\n"
		"^ power               raise top of stack to power and replace\n"
		"q                     quit\n\n");
	break;
      default:
         printf("Unknown Command '%c'\n",c);
         break;
      }
   }

   return 0;
}

 
