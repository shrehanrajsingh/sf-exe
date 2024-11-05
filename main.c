#include "ntvs.h"

#define FBUF_CAP 512
#define PATHBUF_SIZE 1024

#if defined(__unix__) || defined(__APPLE__)
#include <limits.h>
#include <unistd.h>
char *getcwd (char *buf, size_t size);

#elif defined(_WIN32)
#include <windows.h>
DWORD WINAPI GetCurrentDirectory (_In_ DWORD nBufferLength,
                                  _Out_ LPTSTR lpBuffer);
#endif

int
main (int argc, char **argv)
{
  sf_ot_init ();
  sf_fun_init ();
  sf_array_init ();
  sf_module_init ();
  sf_class_init ();
  sf_parser_init ();
  sf_nm_init ();

#if !defined(SF_NODEBUG)
  sf_dbg_fledump_init ();
#endif

  if (argc < 2)
    {
      printf ("enter a valid file path.\n");
      exit (-1);
    }

  char currentDirectory[PATHBUF_SIZE];

#if defined(__unix__) || defined(__APPLE__)
  getcwd (currentDirectory, sizeof (currentDirectory));

#else
  DWORD res = GetCurrentDirectory (PATHBUF_SIZE, currentDirectory);

  if (!(res > 0 && res < PATHBUF_SIZE))
    {
      printf ("very large path.\n");
      exit (-1);
    }
#endif

  const char *filename = argv[1];
  FILE *fptr = fopen (filename, "r");

  if (fptr == NULL)
    {
      printf ("file '%s' does not exist.\n", filename);
      exit (-1);
    }

  size_t cap = FBUF_CAP;
  size_t size = 0;
  char *fcont = sfmalloc (cap * sizeof (char));
  char c;

  while ((c = fgetc (fptr)) != EOF)
    {
      if (cap == size)
        {
          cap += FBUF_CAP;
          fcont = sfrealloc (fcont, cap * sizeof (char));
        }

      fcont[size++] = c;
    }

  if (cap == size)
    fcont = sfrealloc (fcont, (cap + 1) * sizeof (char));

  fcont[size++] = '\0';
  sf_module_addlookuppath (currentDirectory);

  tok_t *token_list = sf_tokenizer_gen (fcont);

  // for (size_t i = 0; token_list[i].type != TOK_EOF; i++)
  //   {
  //     sf_tokenizer_print (token_list[i]);
  //   }

  size_t st_size = 0;
  stmt_t *stmt_tree = sf_ast_stmtgen (token_list, &st_size);

  mod_t *m = sf_mod_new (MOD_TYPE_FILE, NULL);
  add_ntvs (m);

  mod_t *mod = sf_mod_new (MOD_TYPE_FILE, m);
  mod->body = stmt_tree;
  mod->body_len = st_size;

  sf_parser_exec (mod);

  sf_mod_free (mod);
  sf_mod_free (m);

  fclose (fptr);

#if !defined(SF_NODEBUG)
  sf_dbg_dumpclose ();
#endif
  sf_module_dest ();

  return 0;
}
