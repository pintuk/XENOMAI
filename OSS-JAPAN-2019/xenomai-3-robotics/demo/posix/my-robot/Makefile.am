demodir = @XENO_DEMO_DIR@

CCLD = $(top_srcdir)/scripts/wrap-link.sh $(CC)

VERSION_STRING = 0.92

demo_PROGRAMS = my_robot_server

cyclictest_CPPFLAGS = 				\
	$(XENO_USER_CFLAGS)			\
	-I$(top_srcdir)/include			\
	-DVERSION_STRING=$(VERSION_STRING)	\
	-Wno-strict-prototypes			\
	-Wno-implicit-function-declaration	\
	-Wno-missing-prototypes			\
	-Wno-nonnull				\
	-Wno-unused-function

my_robot_server_SOURCES =	\
	src/bt-robot-server.c	\
	src/gpioutils.c		\
	src/gpioutils.h		\
	src/motor_control.c	\
	src/motor_control.h	\
	src/bt_serial.c 	\
	src/bt_serial.h		\
	src/proxi_sensor.c 	\
	src/proxi_sensor.h	\
	src/led_control.c 	\
	src/led_control.h	\
	src/switch_control.c 	\
	src/switch_control.h	\
	src/ultrasonic_sensor.c	\
	src/ultrasonic_sensor.h


my_robot_server_LDFLAGS = @XENO_AUTOINIT_LDFLAGS@ $(XENO_POSIX_WRAPPERS)

my_robot_server_LDADD =		\
	@XENO_CORE_LDADD@	\
	@XENO_USER_LDADD@ 	\
	-lpthread -lrt -lm
