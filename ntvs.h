#include <sunflower.h>

llnode_t *
put_fun_rt (mod_t *mod)
{
  obj_t *arg = (obj_t *)sf_mod_getVar (mod, "arg")->val;

  char *p;
  int c = 0;
  c += printf ("%s", p = sf_parser_objRepr (mod, arg));

  sffree (p);

  obj_t *r = sf_ast_objnew (OBJ_CONST);
  r->v.o_const.type = CONST_INT;
  r->v.o_const.v.c_int.v = c;

  return sf_ot_addobj (r);
}

llnode_t *
putln_fun_rt (mod_t *mod)
{
  llnode_t *r = put_fun_rt (mod);
  putchar ('\n');

  return r;
}

llnode_t *
input_fun_rt (mod_t *mod)
{
  obj_t *msg = (obj_t *)sf_mod_getVar (mod, "msg")->val;

  char *p;
  printf ("%s", p = sf_parser_objRepr (mod, msg));

  sffree (p);

  sf_charptr inp = sf_str_new_empty ();
  char c;

  while ((c = getchar ()) != '\n')
    {
      sf_str_pushchr (&inp, c);
    }

  obj_t *r = sf_ast_objnew (OBJ_CONST);
  r->v.o_const.type = CONST_STRING;
  r->v.o_const.v.c_string.v = inp;

  return sf_ot_addobj (r);
}

/* deprecated */
llnode_t *
ctr_fun_rt (mod_t *mod)
{
  static int c = 0;

  obj_t *r = sf_ast_objnew (OBJ_CONST);
  r->v.o_const.type = CONST_INT;
  r->v.o_const.v.c_int.v = c++;

  return sf_ot_addobj (r);
}

llnode_t *
ord_fun_rt (mod_t *mod)
{
  obj_t *s = (obj_t *)sf_mod_getVar (mod, "str")->val;

  assert (s->type == OBJ_CONST && s->v.o_const.type == CONST_STRING);
  const char *p = SFCPTR_TOSTR (s->v.o_const.v.c_string.v);

  assert (strlen (p) == 1);
  char c = *p;

  obj_t *r = sf_ast_objnew (OBJ_CONST);
  r->v.o_const.type = CONST_INT;
  r->v.o_const.v.c_int.v = (int)c;

  return sf_ot_addobj (r);
}

llnode_t *
super_fun_rt (mod_t *mod)
{
  obj_t *a1 = (obj_t *)sf_mod_getVar (mod, "inst")->val;
  obj_t *a2 = (obj_t *)sf_mod_getVar (mod, "cl")->val;

  assert (a1->type == OBJ_CLASSOBJ
          && "argument 1 of super () is not a class object.");

  assert (a2->type == OBJ_CLASS
          && "argument 2 of super () is not a class instance");

  class_t *a1_c = a1->v.o_cobj.val, *a2_c = a2->v.o_class.val;

  for (size_t i = 0; i < a1_c->il_c; i++)
    {
      obj_t *c = (obj_t *)a1_c->inh_list[i]->val;

      if (c->v.o_class.val->meta.clref == a2_c)
        {
          return a1_c->inh_list[i];
        }
    }

  obj_t *r = sf_ast_objnew (OBJ_CONST);
  r->v.o_const.type = CONST_NONE;

  return sf_ot_addobj (r);
}

llnode_t *
nativemethod_type_str_name_operator_plus (mod_t *mod)
{
  obj_t *self = (obj_t *)sf_mod_getVar (mod, "self")->val;
  obj_t *other = (obj_t *)sf_mod_getVar (mod, "other")->val;

  char *p1, *p2;
  p1 = sf_parser_objRepr (mod, self);
  p2 = sf_parser_objRepr (mod, other);

  sf_charptr res = sf_str_new_empty ();
  sf_str_push (&res, p1);
  sf_str_push (&res, p2);

  obj_t *o = sf_ast_objnew (OBJ_CONST);
  o->v.o_const.type = CONST_STRING;
  o->v.o_const.v.c_string.v = res;

  sffree (p1);
  sffree (p2);

  return sf_ot_addobj (o);
}

llnode_t *
nativemethod_type_str_name_replace (mod_t *mod)
{
  obj_t *self = (obj_t *)sf_mod_getVar (mod, "self")->val,
        *arg1 = (obj_t *)sf_mod_getVar (mod, "a1")->val,
        *arg2 = (obj_t *)sf_mod_getVar (mod, "a2")->val;

  char *s = SFCPTR_TOSTR (self->v.o_const.v.c_string.v),
       *a1 = SFCPTR_TOSTR (arg1->v.o_const.v.c_string.v),
       *a2 = SFCPTR_TOSTR (arg2->v.o_const.v.c_string.v);

  size_t sl = strlen (s), a1l = strlen (a1), a2l = strlen (a2);
  size_t p = 0;

  sf_charptr r = sf_str_new_empty ();

  for (size_t i = 0; i < sl; i++)
    {
      if (!strncmp (s + i, a1, a1l))
        {
          sf_str_push (&r, a2);
          i += a1l - 1;
        }
      else
        sf_str_pushchr (&r, s[i]);
    }

  sf_str_free (&self->v.o_const.v.c_string.v);
  self->v.o_const.v.c_string.v = r;

  obj_t *res = sf_ast_objnew (OBJ_CONST);
  res->v.o_const.type = CONST_NONE;

  return sf_ot_addobj (res);
}

/* deprecated */
llnode_t *
nativemethod_type_int_name_speak (mod_t *mod)
{
  obj_t *self = (obj_t *)sf_mod_getVar (mod, "self")->val;

  char *p = sf_parser_objRepr (mod, self);
  printf ("%s\n", p);

  sffree (p);

  obj_t *o = sf_ast_objnew (OBJ_CONST);
  o->v.o_const.type = CONST_NONE;

  return sf_ot_addobj (o);
}

llnode_t *
nativemethods_type_int_name_bin (mod_t *mod)
{
  obj_t *self = (obj_t *)sf_mod_getVar (mod, "self")->val;

  sf_int i = self->v.o_const.v.c_int.v;
  sf_charptr rs = sf_str_new_empty ();

  while (i != 0)
    {
      sf_str_pushchr (&rs, i % 2 + '0');
      i /= 2;
    }

  sf_str_push (&rs, "b0");
  sf_str_reverse (&rs);

  obj_t *res = sf_ast_objnew (OBJ_CONST);
  res->v.o_const.type = CONST_STRING;
  res->v.o_const.v.c_string.v = rs;

  return sf_ot_addobj (res);
}

llnode_t *
nativemethods_type_int_name_oct (mod_t *mod)
{
  obj_t *self = (obj_t *)sf_mod_getVar (mod, "self")->val;

  sf_int i = self->v.o_const.v.c_int.v;
  sf_charptr rs = sf_str_new_empty ();

  while (i != 0)
    {
      sf_str_pushchr (&rs, i % 8 + '0');
      i /= 8;
    }

  sf_str_push (&rs, "o0");
  sf_str_reverse (&rs);

  obj_t *res = sf_ast_objnew (OBJ_CONST);
  res->v.o_const.type = CONST_STRING;
  res->v.o_const.v.c_string.v = rs;

  return sf_ot_addobj (res);
}

llnode_t *
nativemethods_type_int_name_hex (mod_t *mod)
{
  obj_t *self = (obj_t *)sf_mod_getVar (mod, "self")->val;

  sf_int i = self->v.o_const.v.c_int.v;
  sf_charptr rs = sf_str_new_empty ();

  char q;
  while (i != 0)
    {
      q = i % 16;
      if (q > 9)
        q = 'a' + (q - 10);
      else
        q += '0';

      sf_str_pushchr (&rs, q);
      i /= 16;
    }

  sf_str_push (&rs, "x0");
  sf_str_reverse (&rs);

  obj_t *res = sf_ast_objnew (OBJ_CONST);
  res->v.o_const.type = CONST_STRING;
  res->v.o_const.v.c_string.v = rs;

  return sf_ot_addobj (res);
}

void
add_ntvs (mod_t *m)
{
  /********************************************* */
  /* put(arg) function */

  {
    mod_t *put_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);
    obj_t *arg_obj = sf_ast_objnew (OBJ_CONST);

    arg_obj->type = OBJ_CONST;
    arg_obj->v.o_const.type = CONST_STRING;
    arg_obj->v.o_const.v.c_string.v = sf_str_new_fromStr ("\n");

    sf_mod_addVar (put_mod, "arg", sf_ot_addobj (arg_obj));

    fun_t *put_fun = sf_fun_new ("put", SF_FUN_NATIVE, put_mod, put_fun_rt);

    sf_fun_addarg (put_fun, "arg");

    fun_t *pf = sf_fun_add (put_fun);

    obj_t *ar_obj = sf_ast_objnew (OBJ_FUN);
    ar_obj->type = OBJ_FUN;
    ar_obj->v.o_fun.f = pf;

    sf_mod_addVar (m, "put", sf_ot_addobj (ar_obj));
  }

  /********************************************* */

  /********************************************* */
  /* putln(arg) function */

  {
    mod_t *putln_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);
    obj_t *arg_obj = sf_ast_objnew (OBJ_CONST);

    arg_obj->v.o_const.type = CONST_STRING;
    arg_obj->v.o_const.v.c_string.v = sf_str_new_fromStr ("\n");

    sf_mod_addVar (putln_mod, "arg", sf_ot_addobj (arg_obj));

    fun_t *putln_fun
        = sf_fun_new ("putln", SF_FUN_NATIVE, putln_mod, putln_fun_rt);

    sf_fun_addarg (putln_fun, "arg");

    fun_t *pf = sf_fun_add (putln_fun);

    obj_t *plr_obj = sf_ast_objnew (OBJ_FUN);
    plr_obj->v.o_fun.f = pf;

    sf_mod_addVar (m, "putln", sf_ot_addobj (plr_obj));
  }

  /********************************************* */

  /********************************************* */
  /* input(arg) function */

  {
    mod_t *inp_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);
    obj_t *msg_obj = sf_ast_objnew (OBJ_CONST);

    msg_obj->v.o_const.type = CONST_STRING;
    msg_obj->v.o_const.v.c_string.v = sf_str_new_fromStr ("");

    sf_mod_addVar (inp_mod, "msg", sf_ot_addobj (msg_obj));

    fun_t *inp_fun
        = sf_fun_new ("input", SF_FUN_NATIVE, inp_mod, input_fun_rt);

    sf_fun_addarg (inp_fun, "msg");

    fun_t *ifp = sf_fun_add (inp_fun);

    obj_t *ir_obj = sf_ast_objnew (OBJ_FUN);
    ir_obj->v.o_fun.f = ifp;

    sf_mod_addVar (m, "input", sf_ot_addobj (ir_obj));
  }

  /********************************************* */

  /********************************************* */
  /* ctr() function */

  {
    mod_t *ctr_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);

    fun_t *ctr_fun = sf_fun_new ("ctr", SF_FUN_NATIVE, ctr_mod, ctr_fun_rt);

    fun_t *ctrp = sf_fun_add (ctr_fun);

    obj_t *ctr_obj = sf_ast_objnew (OBJ_FUN);
    ctr_obj->type = OBJ_FUN;
    ctr_obj->v.o_fun.f = ctrp;

    sf_mod_addVar (m, "ctr", sf_ot_addobj (ctr_obj));
  }

  /********************************************* */

  /********************************************* */
  /* super() function */

  {
    mod_t *super_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);
    obj_t *inst_obj = sf_ast_objnew (OBJ_CONST);
    obj_t *cl_obj = sf_ast_objnew (OBJ_CONST);

    inst_obj->v.o_const.type = CONST_NONE;
    cl_obj->v.o_const.type = CONST_NONE;

    sf_mod_addVar (super_mod, "inst", sf_ot_addobj (inst_obj));
    sf_mod_addVar (super_mod, "cl", sf_ot_addobj (cl_obj));

    fun_t *super_fun
        = sf_fun_new ("super", SF_FUN_NATIVE, super_mod, super_fun_rt);

    sf_fun_addarg (super_fun, "inst");
    sf_fun_addarg (super_fun, "cl");

    fun_t *sfp = sf_fun_add (super_fun);

    obj_t *sp_obj = sf_ast_objnew (OBJ_FUN);
    sp_obj->v.o_fun.f = sfp;

    sf_mod_addVar (m, "super", sf_ot_addobj (sp_obj));
  }

  /********************************************* */

  /********************************************* */
  /* STRING METHODS */
  /* ''.operator+() function */

  {
    mod_t *sop_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);

    fun_t *sop_fun = sf_fun_new (SF_OP_OVRLD_PLUS, SF_FUN_NATIVE, sop_mod,
                                 nativemethod_type_str_name_operator_plus);

    sf_fun_addarg (sop_fun, "self");
    sf_fun_addarg (sop_fun, "other");

    fun_t *sopf = sf_fun_add (sop_fun);

    sf_nm_add (SF_OP_OVRLD_PLUS, CONST_STRING, sopf);
  }

  {
    mod_t *rp_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);

    fun_t *rp_fun = sf_fun_new ("replace", SF_FUN_NATIVE, rp_mod,
                                nativemethod_type_str_name_replace);

    sf_fun_addarg (rp_fun, "self");
    sf_fun_addarg (rp_fun, "a1");
    sf_fun_addarg (rp_fun, "a2");

    fun_t *sopf = sf_fun_add (rp_fun);

    sf_nm_add ("replace", CONST_STRING, sopf);
  }

  /********************************************* */

  /********************************************* */
  /* ord(str) function */

  {
    mod_t *ord_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);
    obj_t *str_obj = sf_ast_objnew (OBJ_CONST);

    str_obj->v.o_const.type = CONST_STRING;
    str_obj->v.o_const.v.c_string.v = sf_str_new_fromStr ("");

    sf_mod_addVar (ord_mod, "str", sf_ot_addobj (str_obj));

    fun_t *inp_fun = sf_fun_new ("ord", SF_FUN_NATIVE, ord_mod, ord_fun_rt);

    sf_fun_addarg (inp_fun, "str");

    fun_t *ofp = sf_fun_add (inp_fun);

    obj_t *or_obj = sf_ast_objnew (OBJ_FUN);
    or_obj->v.o_fun.f = ofp;

    sf_mod_addVar (m, "ord", sf_ot_addobj (or_obj));
  }

  /********************************************* */

  /********************************************* */
  /* INT METHODS */

  {
    mod_t *ispeak_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);

    fun_t *isk_fun = sf_fun_new ("speak", SF_FUN_NATIVE, ispeak_mod,
                                 nativemethod_type_int_name_speak);

    sf_fun_addarg (isk_fun, "self");

    sf_nm_add ("speak", CONST_INT, isk_fun);
  }

  {
    mod_t *itb_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);

    fun_t *itb_fun = sf_fun_new ("bin", SF_FUN_NATIVE, itb_mod,
                                 nativemethods_type_int_name_bin);

    sf_fun_addarg (itb_fun, "self");

    sf_nm_add ("bin", CONST_INT, itb_fun);
  }

  {
    mod_t *ito_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);

    fun_t *ito_fun = sf_fun_new ("oct", SF_FUN_NATIVE, ito_mod,
                                 nativemethods_type_int_name_oct);

    sf_fun_addarg (ito_fun, "self");

    sf_nm_add ("oct", CONST_INT, ito_fun);
  }

  {
    mod_t *ith_mod = sf_mod_new (MOD_TYPE_FUNC, NULL);

    fun_t *ith_fun = sf_fun_new ("hex", SF_FUN_NATIVE, ith_mod,
                                 nativemethods_type_int_name_hex);

    sf_fun_addarg (ith_fun, "self");

    sf_nm_add ("hex", CONST_INT, ith_fun);
  }

  /********************************************* */

  sf_module_setparent (m);
}