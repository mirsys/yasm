/**
 * \file libyasm/objfmt.h
 * \brief YASM object format module interface.
 *
 * \rcs
 * $Id$
 * \endrcs
 *
 * \license
 *  Copyright (C) 2001  Peter Johnson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND OTHER CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR OTHER CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * \endlicense
 */
#ifndef YASM_OBJFMT_H
#define YASM_OBJFMT_H

#ifndef YASM_DOXYGEN
/** Base #yasm_objfmt structure.  Must be present as the first element in any
 * #yasm_objfmt implementation.
 */
typedef struct yasm_objfmt_base {
    /** #yasm_objfmt_module implementation for this object format. */
    const struct yasm_objfmt_module *module;
} yasm_objfmt_base;
#endif

/** Version number of #yasm_objfmt_module interface.  Any functional change to
 * the #yasm_objfmt_module interface should simultaneously increment this
 * number.  This version should be checked by #yasm_objfmt loaders to verify
 * that the expected version (the version defined by its libyasm header files)
 * matches the loaded module version (the version defined by the module's
 * libyasm header files).  Doing this will ensure that the module version's
 * function definitions match the module loader's function definitions.  The
 * version number must never be decreased.
 */
#define YASM_OBJFMT_VERSION	1

/** YASM object format module interface. */
typedef struct yasm_objfmt_module {
    /** Version (see #YASM_OBJFMT_VERSION).  Should always be set to
     * #YASM_OBJFMT_VERSION by the module source and checked against
     * #YASM_OBJFMT_VERSION by the module loader.
     */
    unsigned int version;

    /** One-line description of the object format. */
    const char *name;

    /** Keyword used to select object format. */
    const char *keyword;

    /** Default output file extension (without the '.').
     * NULL means no extension, with no '.', while "" includes the '.'.
     */
    /*@null@*/ const char *extension;

    /** Default (starting) section name. */
    const char *default_section_name;

    /** Default (starting) x86 BITS setting.  This only appies to the x86
     * architecture; other architectures ignore this setting.
     */
    const unsigned char default_x86_mode_bits;

    /** NULL-terminated list of debug format (yasm_dbgfmt) keywords that are
     * valid to use with this object format.  The null debug format
     * (null_dbgfmt, "null") should always be in this list so it's possible to
     * have no debug output.
     */
    const char **dbgfmt_keywords;

    /** Default debug format keyword (set even if there's only one available to
     * use).
     */
    const char *default_dbgfmt_keyword;

    /** Create object format.
     * Module-level implementation of yasm_objfmt_create().
     * Call yasm_objfmt_create() instead of calling this function.
     * \param in_filename	main input filename (e.g. "file.asm")
     * \param object		object
     * \param a			architecture in use
     * \return NULL if architecture/machine combination not supported.
     */
    /*@null@*/ /*@only@*/ yasm_objfmt * (*create)
	(const char *in_filename, yasm_object *object, yasm_arch *a);

    /** Module-level implementation of yasm_objfmt_output().
     * Call yasm_objfmt_output() instead of calling this function.
     */
    void (*output) (yasm_objfmt *of, FILE *f, const char *obj_filename,
		    int all_syms, yasm_dbgfmt *df);

    /** Module-level implementation of yasm_objfmt_destroy().
     * Call yasm_objfmt_destroy() instead of calling this function.
     */
    void (*destroy) (/*@only@*/ yasm_objfmt *objfmt);

    /** Module-level implementation of yasm_objfmt_section_switch().
     * Call yasm_objfmt_section_switch() instead of calling this function.
     */
    /*@observer@*/ /*@null@*/ yasm_section *
	(*section_switch)(yasm_objfmt *objfmt, yasm_valparamhead *valparams,
			  /*@null@*/ yasm_valparamhead *objext_valparams,
			  unsigned long line);

    /** Module-level implementation of yasm_objfmt_extern_declare().
     * Call yasm_objfmt_extern_declare() instead of calling this function.
     */
    yasm_symrec * (*extern_declare)
	(yasm_objfmt *objfmt, const char *name,
	 /*@null@*/ yasm_valparamhead *objext_valparams, unsigned long line);

    /** Module-level implementation of yasm_objfmt_global_declare().
     * Call yasm_objfmt_global_declare() instead of calling this function.
     */
    yasm_symrec * (*global_declare)
	(yasm_objfmt *objfmt, const char *name,
	 /*@null@*/ yasm_valparamhead *objext_valparams, unsigned long line);

    /** Module-level implementation of yasm_objfmt_common_declare().
     * Call yasm_objfmt_common_declare() instead of calling this function.
     */
    yasm_symrec * (*common_declare)
	(yasm_objfmt *objfmt, const char *name, /*@only@*/ yasm_expr *size,
	 /*@null@*/ yasm_valparamhead *objext_valparams, unsigned long line);

    /** Module-level implementation of yasm_objfmt_directive().
     * Call yasm_objfmt_directive() instead of calling this function.
     */
    int (*directive) (yasm_objfmt *objfmt, const char *name,
		      yasm_valparamhead *valparams,
		      /*@null@*/ yasm_valparamhead *objext_valparams,
		      unsigned long line);
} yasm_objfmt_module;

/** Create object format.
 * \param module	object format module
 * \param in_filename	main input filename (e.g. "file.asm")
 * \param object	object
 * \param a		architecture in use
 * \return NULL if architecture/machine combination not supported.
 */
/*@null@*/ /*@only@*/ yasm_objfmt *yasm_objfmt_create
    (const yasm_objfmt_module *module, const char *in_filename,
     yasm_object *object, yasm_arch *a);

/** Write out (post-optimized) sections to the object file.
 * This function may call yasm_symrec_* functions as necessary (including
 * yasm_symrec_traverse()) to retrieve symbolic information.
 * \param objfmt	object format
 * \param f		output object file
 * \param obj_filename	output filename (e.g. "file.o")
 * \param all_syms	if nonzero, all symbols should be included in
 *			the object file
 * \param df		debug format in use
 */
void yasm_objfmt_output(yasm_objfmt *objfmt, FILE *f, const char *obj_filename,
			int all_syms, yasm_dbgfmt *df);

/** Cleans up any allocated object format memory.
 * \param objfmt	object format
 */
void yasm_objfmt_destroy(/*@only@*/ yasm_objfmt *objfmt);

/** Switch object file sections.  The first val of the valparams should
 * be the section name.  Calls yasm_object_get_general() to actually get
 * the section.
 * \param objfmt		object format
 * \param valparams		value/parameters
 * \param objext_valparams	object format-specific value/parameters
 * \param line			virtual line (from yasm_linemap)
 * \return NULL on error, otherwise new section.
 */
/*@observer@*/ /*@null@*/ yasm_section *yasm_objfmt_section_switch
    (yasm_objfmt *objfmt, yasm_valparamhead *valparams,
     /*@null@*/ yasm_valparamhead *objext_valparams, unsigned long line);

/** Declare an "extern" (importing from another module) symbol.  Should
 * call yasm_symtab_declare().
 * \param objfmt		object format
 * \param name			symbol name
 * \param objext_valparams	object format-specific value/paramaters
 * \param line			virtual line (from yasm_linemap)
 * \return Declared symbol.
 */
yasm_symrec *yasm_objfmt_extern_declare
    (yasm_objfmt *objfmt, const char *name,
     /*@null@*/ yasm_valparamhead *objext_valparams, unsigned long line);

/** Declare a "global" (exporting to other modules) symbol.  Should call
 * yasm_symtab_declare().
 * \param objfmt		object format
 * \param name			symbol name
 * \param objext_valparams	object format-specific value/paramaters
 * \param line			virtual line (from yasm_linemap)
 * \return Declared symbol.
 */
yasm_symrec *yasm_objfmt_global_declare
    (yasm_objfmt *objfmt, const char *name,
     /*@null@*/ yasm_valparamhead *objext_valparams, unsigned long line);

/** Declare a "common" (shared space with other modules) symbol.  Should
 * call yasm_symtab_declare().
 * declaration.
 * \param objfmt		object format
 * \param name			symbol name
 * \param size			common data size
 * \param objext_valparams	object format-specific value/paramaters
 * \param line			virtual line (from yasm_linemap)
 * \return Declared symbol.
 */
yasm_symrec *yasm_objfmt_common_declare
    (yasm_objfmt *objfmt, const char *name, /*@only@*/ yasm_expr *size,
     /*@null@*/ yasm_valparamhead *objext_valparams, unsigned long line);

/** Handle object format-specific directives.
 * \param objfmt		object format
 * \param name			directive name
 * \param valparams		value/parameters
 * \param objext_valparams	object format-specific value/parameters
 * \param line			virtual line (from yasm_linemap)
 * \return Nonzero if directive was not recognized; 0 if directive was
 *         recognized, even if it wasn't valid.
 */
int yasm_objfmt_directive(yasm_objfmt *objfmt, const char *name,
			  yasm_valparamhead *valparams,
			  /*@null@*/ yasm_valparamhead *objext_valparams,
			  unsigned long line);

#ifndef YASM_DOXYGEN

/* Inline macro implementations for objfmt functions */

#define yasm_objfmt_create(module, in_filename, object, a) \
    module->create(in_filename, object, a)

#define yasm_objfmt_output(objfmt, f, obj_fn, all_syms, df) \
    ((yasm_objfmt_base *)objfmt)->module->output(objfmt, f, obj_fn, all_syms, \
						 df)
#define yasm_objfmt_destroy(objfmt) \
    ((yasm_objfmt_base *)objfmt)->module->destroy(objfmt)
#define yasm_objfmt_section_switch(objfmt, vpms, oe_vpms, line) \
    ((yasm_objfmt_base *)objfmt)->module->section_switch(objfmt, vpms, \
							 oe_vpms, line)
#define yasm_objfmt_extern_declare(objfmt, name, oe_vpms, line) \
    ((yasm_objfmt_base *)objfmt)->module->extern_declare(objfmt, name, \
							 oe_vpms, line)
#define yasm_objfmt_global_declare(objfmt, name, oe_vpms, line) \
    ((yasm_objfmt_base *)objfmt)->module->global_declare(objfmt, name, \
							 oe_vpms, line)
#define yasm_objfmt_common_declare(objfmt, name, size, oe_vpms, line) \
    ((yasm_objfmt_base *)objfmt)->module->common_declare(objfmt, name, size, \
							 oe_vpms, line)
#define yasm_objfmt_directive(objfmt, name, vpms, oe_vpms, line) \
    ((yasm_objfmt_base *)objfmt)->module->directive(objfmt, name, vpms, \
						    oe_vpms, line)

#endif

/** Add a default section to an object.
 * \param objfmt    object format
 * \param object    object
 * \return Default section.
 */
yasm_section *yasm_objfmt_add_default_section(yasm_objfmt *objfmt,
					      yasm_object *object);
#endif
