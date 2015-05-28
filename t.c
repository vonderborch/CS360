/************* t.c file ********************/

// FILL IN  #include files
//int get_ebp();

main(int argc, char *argv[], char *env[])
{
  int a,b,c;
  printf("enter main\n");
  
  printf("&argc=%x\nargv=%x\nenv=%x\n", &argc, argv, env);
  printf("&a=%x\n%b=%x\n&c=%x\n", &a, &b, &c);

  a=1; b=2; c=3;
  A(a,b);
  printf("exit main\n");
}

int A(int x, int y)
{
  int d,e,f;
  printf("enter A\n");
  // PRINT ADDRESS OF d, e, f
  printf("&d: %x\n&e: %x\n&f: %x\n", &d,&e,&f);

  d=3; e=4; f=5;
  B(d,e);
  printf("exit A\n");
}

int B(int x, int y)
{
  int g,h,i;
  printf("enter B\n");
  // PRINT ADDRESS OF g,h,i
  printf("&g: %x\n&h: %x\n&i: %x\n", &g,&h,&i);

  g=6; h=7; i=8;
  C(g,h);
  printf("exit B\n");
}


int *p, ebp;

int C(int x, int y)
{
  int u, v, w;
  printf("enter C\n");
  // PRINT ADDRESS OF u,v,w;
  printf("&u: %x\n&v: %x\n&w: %x\n", &u,&v,&w);
  
  u=9; v=10; w=11;

  /*********** Write C code to DO ************ 
        (1)-(5) AS SPECIFIED BELOW 
  *******************************************/
  // 1)
  printf("Part 1:\n");
  ebp = get_ebp();   // call get_ebp() in s.s file
  printf("&ebp: %x\n", ebp);
  printf("End Part 1\n");

  // 2)
  p=&w;
  int i = 0;
  for (i=0;i<100;i++)
    {
      printf("Entry - Address: %x, Contents: %x\n", p, *p);
      p++;
    }
  printf("End Part 2\n");

  printf("exit C\n");
}
