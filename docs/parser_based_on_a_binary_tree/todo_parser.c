#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lb_types.h"
#include "lb_parser.h"
#include "lb_algebra.h"
#include "lb_complex.h"

typedef enum {fn_sin_r, fn_cos_r, fn_min_r, fn_sin_c} FN_CODE_T;
typedef enum {vb_float, vb_complex, vb_vector_r, vb_matrix_r, vb_vector_c, vb_matrix_c} VB_CODE_T;



#define FN_INTEGER 0x01
#define FN_REAL    0x02
#define FN_COMPLEX 0x04
#define FN_VECTOR  0x08
#define FN_MATRIX  0x10

#define N_FUNCTIONS 3

typedef struct 
{
  FN_CODE_T fn_code;
  VB_CODE_T vb_code;
  void *value;
} NODE_T;

typedef struct 
{
  VB_CODE_T vb_code;
  void *value;
  U_INT_8_T allocated;
} VAR_T;

typedef struct 
{
  FN_CODE_T fn_code;
  char text[20];
  U_INT_8_T args;  
  U_INT_8_T fn_type;  
} FUNCTION_T;


static FUNCTION_T functions[N_FUNCTIONS] = { 
  {fn_sin_r,"sin",1, FN_REAL+FN_COMPLEX}, 
  {fn_cos_r,"cos",1, FN_REAL+FN_COMPLEX}, 
  {fn_min_r,"min",2, FN_REAL+FN_COMPLEX} }; 

void multiplica(VAR_T *in1, VAR_T *in2, VAR_T *out1)
{
  U_INT_16_T i;
  FLOAT_T f1, f2;
  COMPLEX_T c1, c2, c3;
  VECTOR_R_T v1, v2;

  if ( ((*in1).vb_code==vb_float) && ((*in2).vb_code==vb_float) )
    {
      (*out1).vb_code=vb_float;
      *((FLOAT_T*)(*out1).value)=(*((FLOAT_T*)(*in1).value))*(*((FLOAT_T*)(*in2).value));
      return;
    }
  
  /* Case2: [Float]*[Complex]=[Complex] */
  if ( ((*in1).vb_code==vb_float) && ((*in2).vb_code==vb_complex) ) 
    {
      (*out1).vb_code=vb_complex;
      (*((COMPLEX_T*)(*out1).value))=lb_cp_multiply_real( *(COMPLEX_T*)(*in2).value,
							  *(FLOAT_T*)(*in1).value);
      return;
    }

  /* Case3: [Complex]*[Float]=[Complex] */
  if ( ((*in1).vb_code==vb_complex) && ((*in2).vb_code==vb_float) ) 
    {
      (*out1).vb_code=vb_complex;
      (*((COMPLEX_T*)(*out1).value))=lb_cp_multiply_real( *(COMPLEX_T*)(*in1).value,
							  *(FLOAT_T*)(*in2).value);
      return;
    }
 
  /* Case4: [Complex]*[Complex]=[Complex] */
  if ( ((*in1).vb_code==vb_complex) && ((*in2).vb_code==vb_complex) )
    {
      (*out1).vb_code=vb_complex;
      (*((COMPLEX_T*)(*out1).value))=lb_cp_multiply( *(COMPLEX_T*)(*in1).value,
						     *(COMPLEX_T*)(*in2).value);
      return;
    }

  /* Case5: [Vector_r]*[Float]=[Vector_r] */
  if ( ((*in1).vb_code==vb_vector_r) && ((*in2).vb_code==vb_float) )
    {

      (*out1).vb_code=vb_vector_r;
 
      (*(VECTOR_R_T*)(*out1).value).items=(*(VECTOR_R_T*)(*in1).value).items;

      if ((*out1).allocated)
	  lb_al_release_vector_r((VECTOR_R_T*)(*out1).value);
      
      lb_al_create_vector_r((VECTOR_R_T*)(*out1).value);
      (*out1).allocated=1;

      lb_al_multiply_vector_r_float( (VECTOR_R_T*)(*in1).value,
				     *(FLOAT_T*)(*in2).value,
				     (VECTOR_R_T*)(*out1).value);
      return;
    }

  /* Case6: [Float]*[Vector_r]=[Vector_r] */
  if ( ((*in1).vb_code==vb_float) && ((*in2).vb_code==vb_vector_r) )
    {

      (*out1).vb_code=vb_vector_r;
 
      (*(VECTOR_R_T*)(*out1).value).items=(*(VECTOR_R_T*)(*in2).value).items;

      if ((*out1).allocated)
	  lb_al_release_vector_r((VECTOR_R_T*)(*out1).value);
      
      lb_al_create_vector_r((VECTOR_R_T*)(*out1).value);
      (*out1).allocated=1;

      lb_al_multiply_vector_r_float( (VECTOR_R_T*)(*in2).value,
				     *(FLOAT_T*)(*in1).value,
				     (VECTOR_R_T*)(*out1).value);
      return;
    }

  /* Case7: [Vector_r]*[Vector_r]=[Vector_r] */
  if ( ((*in1).vb_code==vb_float) && ((*in2).vb_code==vb_vector_r) )
    {

      (*out1).vb_code=vb_vector_r;
 
      (*(VECTOR_R_T*)(*out1).value).items=(*(VECTOR_R_T*)(*in2).value).items;

      if ((*out1).allocated)
	  lb_al_release_vector_r((VECTOR_R_T*)(*out1).value);
      
      lb_al_create_vector_r((VECTOR_R_T*)(*out1).value);
      (*out1).allocated=1;

      lb_al_multiply_vector_r_float( (VECTOR_R_T*)(*in2).value,
				     *(FLOAT_T*)(*in1).value,
				     (VECTOR_R_T*)(*out1).value);
      return;
    }


  printf ("Case not defined:  Multiply \r\n"); 
}

void print_var(VAR_T *V)
{
  switch ((*V).vb_code)
    {
    case vb_float: 
      printf("FLOAT = %f\r\n",*(FLOAT_T*)(*V).value);
      break;
    case vb_complex: 
      printf("COMPLEX = %f  , %f\r\n",(*(COMPLEX_T*)(*V).value).r,(*(COMPLEX_T*)(*V).value).i);
      break;
    case vb_vector_r: 
      printf("VECTOR = \r\n");
      lb_al_print_vector_r((VECTOR_R_T*)(*V).value,"v",FLOAT_FORMAT);
      break;
    case vb_matrix_r: 
      break;
      }
}

void playing(void)
{
  FLOAT_T F1, F2, F3;
  COMPLEX_T C1, C2, C3;
  VECTOR_R_T V1, V2, V3;
  MATRIX_R_T M1, M2, M3;
  VAR_T par1, par2, par3;
  U_INT_16_T i;

  F1=10;
  F2=2;

  C1.r=3.1416;
  C1.i=-12.54;

  C2.r= 21.1;
  C2.i= 4.75;

  V1.items=30; lb_al_create_vector_r(&V1);
  V2.items=3; lb_al_create_vector_r(&V2);
  V3.items=3; lb_al_create_vector_r(&V3);

  for (i=0;i<V1.items;i++)
    V1.array[i]=i;

  par1.vb_code=vb_vector_r;
  par1.value=&V1;

  par2.vb_code=vb_float;
  // par2.value=(COMPLEX_T*)malloc(sizeof(COMPLEX_T));
  par2.value=&F1;

  par3.vb_code=vb_float;

  M1.rows=2;  
  M1.cols=2;
  lb_al_create_matrix_r(&M1);

  M2.rows=2;  
  M2.cols=2;
  lb_al_create_matrix_r(&M2);

  M3.rows=2;  
  M3.cols=2;
  lb_al_create_matrix_r(&M3);
  

  //  printf("Result was a Complex = %f  , %f\r\n",(*(COMPLEX_T*)par3.value).r,(*(COMPLEX_T*)par3.value).i);

  print_var(&par1);
  print_var(&par2);
  multiplica(&par1,&par2,&par3);
  multiplica(&par1,&par2,&par3);
  print_var(&par3);

  
  lb_al_release_vector_r(&V1);
  lb_al_release_vector_r(&V2);
  lb_al_release_vector_r(&V3);

  lb_al_release_matrix_r(&M1);
  lb_al_release_matrix_r(&M2);
  lb_al_release_matrix_r(&M3);
}


void parse_fn(char *str)
{
  U_INT_16_T i;
  U_INT_8_T flag;
  char substr[100];

  i=0;
  /* Looking for first non-alphabetic character */
  //while ((str[i]!='\0') || isalpha(str[i])) i++;
  while (isalpha(str[i])) 
    {
      substr[i]=str[i];
      i++;
    }
  substr[i]='\0';
  printf("i= %i \r\n",i);
  printf("substr = %s\r\n",substr);
  
  for (i=0;i<N_FUNCTIONS;i++)
    {
      if (!strcmp(functions[i].text,substr))
	printf("match [%i]=%i\r\n",i,strcmp(functions[i].text,substr));
    }

}



#ifdef NADA


static struct FUNCTION_S funciones[3] = { {"sin",1}, {"cos",1}, {"min",2}}; 


it ParseC; { Solo PARSEC4.PAS (Analizador) y Math2.PAS (Libreria) } 
 
interface 
 
uses Math, PCommon; 
 
type 
  NodeTypeCx = (AbsNd, ReNd, ImNd, ArgNd, SqrtNd, IntNd, U1Nd, U2Nd, 
                              SgnNd, RoundNd, TruncNd, FracNd, 
                              SinNd, CosNd, TanNd, ASinNd, ACosNd, ATanNd, 
                SinhNd, CoshNd, TanhNd, ASinhNd, ACoshNd, ATanhNd, 
                ExpNd, LnNd, LogNd, NotNd, MinNd, MaxNd, LogxNd, IFNd, 
                SimpsonNd, PiNd, iNd, ENod, C1Nd, C2Nd, C3nd, 
                                PosNd, NegNd, PlusNd, MinusNd, MulNd, DivNd, PwrNd, 
                  EqnNd,AndNd, OrNd, BiggerNd, BigOqNd, 
                  SmallerNd, SmllOqNd, ConstNd); 
 
  TreeCx = ^TreeRecCx; 
 
  TreeRecCx = record 
              case typ: NodeTypeCx of 
                PlusNd    : (left, right, right2, right3: TreeCx; Nd:NodeTypeCx); 
                ConstNd    : (rl: TComplex); 
            end; 
 
function ParseExprCx(var inExpr: String; ValidParseVars : String): TreeCx; 
function EvalCx(tr: TreeCx; C1, C2, C3: TComplex): TComplex; 
function CSimpson(tr : TreeCx; Ca, Cb, Cm, C1, C2, C3 : TComplex; 
                                    Nodo : NodetypeCx) : TComplex; 
 
implementation 
 
const 
  funTblCx: array[AbsNd..ENod] of String[7] = 
  ('abs','re','im','arg','sqrt','int','u1','u2','sgn','round','trunc','frac', 
   'sin','cos','tan','asin','acos','atan','sinh','cosh','tanh','asinh', 
   'acosh','atanh','exp','ln','log','not','min','max','logx','if', 
   'simpson','pi','i','e'); 
 
var 
  laValCx: record case Integer of 
               0: (rl: TComplex); 
               2: (typ: NodeTypeCx); 
             end; 
 
  SVars : String; 
 
function CSimpson(tr : TreeCx; Ca, Cb, Cm, C1, C2, C3 : TComplex; 
                                    Nodo : NodetypeCx) : TComplex; 
var delta, x, a, b, m, suma     : Float; 
        index                                         : Integer; 
    aux                                                : TComplex; 
    res                                             : TCmplx; 
    FlagComplex                             : Boolean; 
begin 
  FlagComplex := False; 
  suma := 0; 
 
  With res do 
  begin { With } 
    len := 2*SizeOf(Float); 
    i := 0; 
    r := 0; 
 
        If (Im(Ca) < MinNum) and 
       (Im(Cb) < MinNum) and 
       (Im(Cm) < MinNum) then 
 
    begin { Ca , Cb y Cm sin parte imaginaria } 
            a := TCmplx(Ca).r; 
            b := TCmplx(Cb).r; 
      m := Round(TCmplx(Cm).r); 
 
      If abs(Frac(m)) < MinNum then 
      begin { m es entero } 
                delta := (b - a)/(2*m); 
 
        case Nodo of 
                  C1Nd : aux := EvalCx(tr,Ca,C2,C3); 
                C2Nd : aux := EvalCx(tr,C1,Ca,C3); 
                C3Nd : aux := EvalCx(tr,C1,C2,Ca); 
              end; 
 
              If abs(Im(aux)) < MinNum then 
              begin { f(a) es real } 
                  suma := TCmplx(aux).r; 
 
          case Nodo of 
                      C1Nd : aux := EvalCx(tr,Cb,C2,C3); 
                    C2Nd : aux := EvalCx(tr,C1,Cb,C3); 
                    C3Nd : aux := EvalCx(tr,C1,C2,Cb); 
                  end; 
 
                  If (abs(Im(aux)) < MinNum) then 
                begin { f(b) es real } 
                    suma := suma + Re(aux); 
                index := 1; 
                  repeat 
                            x := a + index*delta; 
 
              case Nodo of 
                              C1Nd : aux := EvalCx(tr,Complex(x,0),C2,C3); 
                            C2Nd : aux := EvalCx(tr,C1,Complex(x,0),C3); 
                            C3Nd : aux := EvalCx(tr,C1,C2,Complex(x,0)); 
                          end; 
 
                    If (abs(TCmplx(aux).i) < MinNum) then 
                    If Odd(index) then 
                                    suma := suma + 4*TCmplx(aux).r 
                            else 
                          suma := suma + 2*TCmplx(aux).r 
                    else 
                    begin 
                        FlagComplex := True; 
                suma:=0; 
                      EvalError(ERRComplexArg); 
                    end; 
                    index := index + 1; 
                  until (index = 2*Round(m)) or FlagComplex; 
            end 
          else 
          begin 
            suma:=0; 
              EvalError(ERRComplexArg); 
          end; 
        end { f(a) es real } 
        else 
        begin 
          suma:=0; 
            EvalError(ERRComplexArg); 
        end; 
              suma:=suma*delta/3; 
        end { m es entero } 
      else { m si tiene parte fraccionaria } 
          EvalError(ERRNotIntegerArg) 
    end  { Ca , Cb y Cm sin parte imaginaria } 
    else { Ca , Cb y Cm con parte imaginaria } 
        EvalError(ERRComplexArg); 
    r := suma; 
  end; { with } 
  CSimpson := TComplex(res) 
end; { Funcion } 
 
procedure lexCx; 
var 
    tok                : String; 
  i, code        : Integer; 
  t                    : NodeTypeCx; 
  dot                : Boolean; 
  aux                : Float; 
begin 
  SetLength(tok,300); 
    while buf[p] in [^I,' '] do inc(p); 
  case buf[p] of 
      #0 : la := EndInput; 
    '(': begin inc(p); la := LParen;                                                     end; 
    ')': begin inc(p); la := RParen;                                                     end; 
    ',': begin inc(p); la := Comma;                                                      end; 
    '+': begin inc(p); la := AddOp;   laValCx.typ := PlusNd;     end; 
    '-': begin inc(p); la := AddOp;   laValCx.typ := MinusNd; end; 
    '*': begin inc(p); la := MulOp;   laValCx.typ := MulNd;     end; 
    '/': begin inc(p); la := MulOp;   laValCx.typ := DivNd;     end; 
    '^': begin inc(p); la := PwrOp;   laValCx.typ := PwrNd;     end; 
    '=': begin inc(p); la := EqnOp;   laValCx.typ := EqnNd;     end; 
    '&': begin inc(p); la := MulOp;   laValCx.typ := AndNd;     end; 
    '|': begin inc(p); la := AddOp;   laValCx.typ := OrNd;         end; 
    '>': begin 
                inc(p); 
          la := EqnOp; 
          if buf[p]='=' then 
          begin 
              inc(p); 
            laValCx.typ := BigOqNd 
          end 
          else 
                 laValCx.typ := BiggerNd; 
             end; 
    '<': begin 
                inc(p); 
                la := EqnOp; 
              if buf[p]='=' then 
          begin 
              inc(p); 
            laValCx.typ := SmllOqNd 
          end 
          else 
                 laValCx.typ := SmallerNd; 
             end; 
    'a'..'z': 
    begin 
        i := 0; 
      repeat 
          inc(i); 
                tok[i] := buf[p]; 
                inc(p); 
      until not (buf[p] in ['a'..'z', '0'..'9']); 
      SetLength(tok,i); 
            t := AbsNd; 
      while (t <= ENod) and (funTblCx[t] <> tok) do inc(t); 
      if t > ENod then 
      begin 
        if Length(tok)=1 then 
        begin 
            case Pos(tok[1],SVars) of 
              1 : begin 
                            la := VarName; 
                    t  := C1Nd; 
                        end; 
            2 : begin 
                            la := VarName; 
                    t  := C2Nd; 
                        end; 
            3 : begin 
                            la := VarName; 
                    t  := C3Nd; 
                        end; 
              else 
                  Error('Unknown variable'); 
          end 
        end 
        else 
            Error('Unknown function'); 
      end 
        else 
              If t < PiNd then 
                  la := FunName 
              else 
                la := VarName; 
      laValCx.typ := t; 
    end; 
    '0'..'9','.': 
    begin 
        dot := False; 
      i := 0; 
      repeat 
          if buf[p] = '.' then 
        if dot then 
            LexError(TooManyDecPoints) 
        else 
            dot := True; 
        inc(i); tok[i] := buf[p]; inc(p); 
      until not (buf[p] in ['.', '0'..'9']); 
      SetLength(tok,i); 
      la := UnsConst; 
      Val(tok, aux, code); 
      laValCx.rl:=Complex(aux,0); 
      if code <> 0 then 
          error('Bad constant'); 
    end; 
    else lexError(UnknownSymbol); 
  end; 
end; 
 
procedure matchCx(t: Token); 
begin 
if la <> t then 
    error('Expected : ' + tokStr[t]); 
  lexCx; 
end; 
 
function makeNodeCx(t: NodeTypeCx; l, r, r2, r3: TreeCx): TreeCx; 
var 
    tr: TreeCx; 
begin 
    New(tr); 
  with tr^ do begin 
      typ := t; left := l; right := r; right2 := r2; right3 := r3 
  end; 
  makeNodeCx := tr; 
end; 
 
function makeConstNodeCx(v: TComplex): TreeCx; 
var 
    tr: TreeCx; 
begin 
    New(tr); 
  with tr^ do begin 
      typ := ConstNd; rl := v; 
  end; 
  makeConstNodeCx := tr; 
end; 
 
 
function EqnCx: TreeCx; forward; 
 
function factorCx: TreeCx; 
var 
    tr    : TreeCx; 
  op    : NodeTypeCx; 
  begin 
      case la of 
      FunName: 
        begin 
            op := laValCx.typ; 
          lexCx; 
        matchCx(Lparen); 
        tr := makeNodeCx(op, EqnCx, nil, nil, nil); 
        if op in [MinNd..SimpsonNd] then     { Dos o mas argumentos } 
        begin 
            matchCx(Comma); 
          tr^.right := EqnCx; 
        end; 
        if op in [IfNd..SimpsonNd] then     { Tres o mas argumentos } 
        begin 
             matchCx(Comma); 
          tr^.right2 := EqnCx; 
        end; 
        if op = SimpsonNd then                     { cuatro argumentos } 
        begin 
          matchCx(Comma); 
          tr^.right3 := EqnCx; 
          matchCx(Comma); 
                    matchCx(VarName); 
          tr^.Nd := laValCx.typ; 
        end; 
        factorCx := tr; 
        matchCx(Rparen); 
      end; 
      VarName: 
          begin  { Vars de una sola letra } 
              factorCx := makeNodeCx(laValCx.typ, nil, nil, nil, nil); 
            lexCx; 
          end; 
 
      UnsConst: 
          begin 
              factorCx := makeConstNodeCx(laValCx.rl); 
            lexCx; 
          end; 
      Lparen: 
          begin 
            lexCx; 
          factorCx := EqnCx; 
          matchCx(Rparen); 
        end; 
    else 
        error('Factor expected'); 
  end; 
end; 
 
function powerCx: TreeCx; 
var 
    tr: TreeCx; 
  op: NodeTypeCx; 
begin 
    tr := factorCx; 
  if la = PwrOp then begin 
      op := laValCx.typ; 
    lexCx; 
    tr := makeNodeCx(op, tr, powerCx, nil, nil); 
  end; 
  powerCx := tr; 
end; 
 
function signedFactCx: TreeCx; 
var 
    sgn: NodeTypeCx; 
begin 
    sgn := PosNd; 
  if la = AddOp then 
  begin 
      if laValCx.typ = MinusNd then sgn := NegNd; 
      lexCx; 
    end; 
    if sgn = NegNd then 
        signedFactCx := makeNodeCx(NegNd, powerCx, nil, nil, nil) 
    else 
      signedFactCx := powerCx; 
end; 
 
function termCx: TreeCx; 
var 
    tr : TreeCx; 
  op : NodeTypeCx; 
begin 
    tr := signedFactCx; 
  while la = MulOp do begin 
      op := laValCx.typ; 
    lexCx; 
    tr := makeNodeCx(op, tr, signedFactCx, nil, nil); 
  end; 
  termCx := tr 
end; 
 
function exprCx: TreeCx; 
var 
    tr : TreeCx; 
  op : NodeTypeCx; 
begin 
    tr := termCx; 
  while la = AddOp do begin 
      op := laValCx.typ; 
    lexCx; 
    tr := makeNodeCx(op, tr, termCx, nil, nil); 
  end; 
  exprCx := tr; 
end; 
 
function EqnCx: TreeCx; 
var 
    tr : TreeCx; 
  op : NodeTypeCx; 
begin 
    tr := ExprCx; 
  while la = EqnOp do 
  begin 
      op := laValCx.typ; 
    lexCx; 
    tr := makeNodeCx(op, tr, ExprCx, nil, nil); 
  end; 
  EqnCx:=tr; 
end; 
 
 
function ParseExprCx(var inExpr: String; ValidParseVars : String): TreeCx; 
var i : integer; 
begin 
    { Inicio de las variables globales } 
  BAnyParseError:=False; 
  ICountParseError:=1; 
 
  for i:=1 to NumMaxErrors do ListParseError[i]:=''; 
  SVars:=ValidParseVars; 
  ToLowCase(SVars); 
  If Length(SVars) <> 0  then 
       For i:=1 to Length(SVars) do 
        if not (SVars[i] in  ['a'..'z']) then 
          Error('Invalid Variable ( ' + SVars[i] + ' )') else 
        if SVars[i] = 'e' then 
          Error('Preasigned constant (e)'); 
 
  SetLength(Buf,300); 
  la:=EndInput; 
    buf := inExpr + #0; 
  ToLowCase(buf); 
  p := 1; 
  lexCx; 
  parseExprCx := EqnCx; 
  matchCx(EndInput); 
end; 
 
function EvalCx(tr: TreeCx; C1, C2, C3: TComplex): TComplex; 
    function e(tr: TreeCx): TComplex; 
  begin 
      with tr^ do 
        case typ of            { Funciones con complejos } 
          AbsNd         : e := CAbs(e(left)); 
        ReNd            : e := CRe(e(left)); 
        ImNd      : e := CIm(e(left)); 
        ArgNd            : e := CArg(e(left)); 
        SqrtNd        :    e := CSqrt(e(left)); 
        IntNd         : If (abs(Im(e(left)))< MinNum) then 
                                    e := Complex(Int(Re(e(left))),0) 
                                else begin 
                        EvalError(ERRComplexArg); 
                      e:=Complex(0,0); 
                    end; 
        U1Nd             : If (abs(Im(e(left)))< MinNum) then 
                                    e := Complex(FU1(Re(e(left))),0) 
                                else begin 
                        EvalError(ERRComplexArg); 
                      e:=Complex(0,0); 
                    end; 
        U2Nd             : If (abs(Im(e(left)))< MinNum) then 
                                    e := Complex(FU2(Re(e(left))),0) 
                                else begin 
                        EvalError(ERRComplexArg); 
                      e:=Complex(0,0); 
                    end; 
        SgnNd         : If (abs(Im(e(left)))< MinNum) then 
                                    e := Complex(FSgn(Re(e(left))),0) 
                                else begin 
                        EvalError(ERRComplexArg); 
                      e:=Complex(0,0); 
                    end; 
        RoundNd        :    If (abs(Im(e(left)))< MinNum) then 
                                    e := Complex(Round(Re(e(left))),0) 
                                else begin 
                        EvalError(ERRComplexArg); 
                      e:=Complex(0,0); 
                    end; 
        TruncNd        :    If (abs(Im(e(left)))< MinNum) then 
                                    e := Complex(Trunc(Re(e(left))),0) 
                                else begin 
                        EvalError(ERRComplexArg); 
                      e:=Complex(0,0); 
                    end; 
        FracNd        :    If (abs(Im(e(left)))< MinNum) then 
                                    e := Complex(Frac(Re(e(left))),0) 
                                else begin 
                        EvalError(ERRComplexArg); 
                      e:=Complex(0,0); 
                    end; 
        SinNd            : e := CSin(e(left)); 
        CosNd         : e := CCos(e(left)); 
        TanNd            : e := CTan(e(left)); 
        ASinNd        : e := CASin(e(left)); 
        ACosNd        : e := CACos(e(left)); 
          ATanNd      : e := CATan(e(left)); 
        SinhNd        : e := CSinh(e(left)); 
        CoshNd        : e := CCosh(e(left)); 
        TanhNd        : e := CTanh(e(left)); 
                ASinhNd        : e := CASinh(e(left)); 
        ACoshNd        : e := CACosh(e(left)); 
        ATanhNd        : e := CATanh(e(left)); 
        ExpNd            : e := CExp(e(left)); 
        LnNd            : e := CLn(e(left)); 
        LogNd            : e := CLog(e(left)); 
        NotNd         :        If (abs(Re(e(left)))> MinNum) or 
                                         (abs(Im(e(left)))> MinNum) 
                                    then e:=Complex(0,0) 
                         else e:=Complex(1,0); 
        MinNd            : e := CMin(e(left),e(right)); 
        MaxNd            : e := CMax(e(left),e(right)); 
        LogxNd    : e := CLogx(e(left),e(right)); 
        IfNd            : If (abs(Re(e(left))) > MinNum) or 
                                     (abs(Im(e(left))) > MinNum) 
                                then e := e(right) else e := e(right2); 
        SimpsonNd    : e := CSimpson(left, e(right),e(right2), e(right3), 
                                            C1, C2, C3, Nd); 
        PiNd            : e := Complex(Pi,0); 
        iNd                : e := Complex(0,1); 
        ENod      : e := Complex(2.71828182846,0); 
        C1Nd            : e := C1; 
        C2Nd            : e := C2; 
        C3Nd            : e    := C3; 
        PosNd            : e := e(left); 
        NegNd            : e := Complex(-Re(e(left)),-Im(e(left))); 
        PlusNd        : e := CSum(e(left),e(right)); 
        MinusNd        : e := CSum(e(left),Complex(-Re(e(right)),-Im(e(right)))); 
        MulNd        : e := CProd(e(left),e(right)); 
        DivNd        : e := CDiv(e(left), e(right)); 
        PwrNd        : e := CPwr(e(left), e(right)); 
        EqnNd            :    if  (abs( abs(Re(e(left))) - abs(Re(e(right)))) < MinNum) 
                     and (abs( abs(Im(e(left))) - abs(Im(e(right)))) < MinNum) 
                     then e := Complex(1,0) else e :=Complex(0,0); 
        AndNd            :    if ((abs(Re(e(left)))> MinNum) or 
                                        (abs(Im(e(left)))> MinNum)) and 
                        ((abs(Re(e(right)))> MinNum) or 
                        (abs(Im(e(right)))> MinNum)) 
                                        then e := Complex(1,0) else e :=Complex(0,0); 
        OrNd            :if (abs(Re(e(left)))> MinNum) or 
                                    (abs(Im(e(left)))> MinNum) or 
                      (abs(Re(e(right)))> MinNum) or 
                      (abs(Im(e(right)))> MinNum) 
                      then e := Complex(1,0) else e :=Complex(0,0); 
        BiggerNd    :    If (Im(e(left)) > MinNum) or (Im(e(right)) > MinNum) then 
                                    begin 
                            EvalError(ERRComplexArg); 
                          e:=Complex(0,0); 
                        end 
                                  else 
                        if Re(e(left)) > Re(e(right)) then e := Complex(1,0) 
                          else e := Complex(0,0); 
        BigOqNd        :    If (Im(e(left)) > MinNum) or (Im(e(right)) > MinNum) then 
                                    begin 
                          EvalError(ERRComplexArg); 
                         e:=Complex(0,0); 
                      end 
                                else 
                        if (Re(e(left)) > Re(e(right))) or 
                         (abs(Re(e(left)) - Re(e(right))) < MinNum) 
                      then e := Complex(1,0) else e := Complex(0,0); 
            SmallerNd    :    If (Im(e(left)) > MinNum) or (Im(e(right)) > MinNum) then 
                        begin 
                            EvalError(ERRComplexArg); 
                          e:=Complex(0,0); 
                        end 
                        else 
                                                if Re(e(left)) < Re(e(right)) then e := Complex(1,0) 
                             else e := Complex(0,0); 
        SmllOqNd    :    If (Im(e(left)) > MinNum) or (Im(e(right)) > MinNum) then 
                                    begin 
                          EvalError(ERRComplexArg); 
                         e:=Complex(0,0); 
                      end 
                                else 
                        if (Re(e(left)) < Re(e(right))) or 
                         (abs(Re(e(left)) - Re(e(right))) < MinNum) 
                      then e := Complex(1,0) else e := Complex(0,0); 
 
        ConstNd            : e := rl; 
      end; 
  end; 
begin { EvalCx } 
      EvalCx := e(tr); 
end; 
 
#endif
