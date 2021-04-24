demodir = @XENO_DEMO_DIR@

CCLD = $(top_srcdir)/scripts/wrap-link.sh $(CC)

demo_PROGRAMS = simple_xeno_task

cppflags = 			\
	$(XENO_USER_CFLAGS)	\
	-I$(top_srcdir)/include

ldadd = 					\
	@XENO_AUTOINIT_LDFLAGS@			\
	$(XENO_POSIX_WRAPPERS)			\
	../../../lib/alchemy/libalchemy.la 	\
	../../../lib/copperplate/libcopperplate.la \
	@XENO_CORE_LDADD@			\
	@XENO_USER_LDADD@			\
	-lrt -lpthread -lm

simple_xeno_task_SOURCES = simple_xeno_task.c
simple_xeno_task_CPPFLAGS = $(cppflags)
simple_xeno_task_LDADD = $(ldadd)
