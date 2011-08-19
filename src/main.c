#include <gtk/gtk.h>
//~ #include <glade/glade.h>
#include <stdio.h>
#include "serialcom.h"

//Serial port variabels
int fd;
const char * device = "/dev/ttyUSB0";

#define ON 1
#define OFF 0

//Motor
//Motor speed min max in [rpm]
#define MOTOR_RPM_MIN 0
#define MOTOR_RPM_MAX 4000							//TODO: Fix
#define MOTOR_RPM_STARTUP_DEFAULT_VALUE 30			//TODO: Match with RAMP_TO_DEFAULT_VALUE?

//Motor current min, max values given in mA
#define MOTOR_CURRENT_MIN 0
#define MOTOR_CURRENT_MAX 250						//TODO: Fix
#define MOTOR_STARTUP_CURRENT_DEFAULT_VALUE 10		//TODO: Fix

typedef enum {
	MOTOR_OFF = 0,
	MOTOR_ON,
	MOTOR_IDLE,
} motor_state_t;

volatile motor_state_t motor_state;
GtkWidget * spin_button_motor_startup_current;
GtkWidget * spin_button_motor_startup_until_rpm;

//Ramp
//Ramp time in [s]
#define RAMP_TIME_MIN 1								//One second ramp time is probalby too fast
#define RAMP_TIME_MAX 3600							//Something silly big
#define RAMP_TIME_DEFAULT_VALUE 10
#define RAMP_FROM_DEFAULT_VALUE 30					//TODO: Match with MOTOR_RPM_STARTUP_DEFAULT_VALUE?
#define RAMP_TO_DEFAULT_VALUE 600

typedef enum {
	RAMP_START,
	RAMP_REVERSE,
	RAMP_STOP,
} ramp_state_t;

GtkWidget * spin_button_ramp_from;
GtkWidget * spin_button_ramp_to;
GtkWidget * spin_button_ramp_time;

//Scale, PMSM speed settings
#define SCALE_DEFAULT_VALUE 40

//Status bar
GtkWidget * statusbar;
int statusbar_context_id;

void on_window_destroy (GtkObject *object, gpointer user_data)
{
	gtk_main_quit();
}

void m_button_quit(GtkWidget * widget, gpointer data)
{
	gtk_main_quit();
}

void print_statusbar(char * str)
{
	gtk_statusbar_remove_all(GTK_STATUSBAR(statusbar), statusbar_context_id);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id, str);
	serialcom_write(&fd, str);
}


/************************************************************************
 * Callback functions for serial connect/disconnect
 ************************************************************************/
static unsigned char connected;
void m_togglebutton_connect_toggled(GtkWidget * widget, gpointer data)
{
	switch(connected) {
		case FALSE:
			g_print("Connecting");
			if(serialcom_init_old(&fd, "/dev/ttyACM0") == OK) {
				connected = TRUE;
			}
			break;

		case TRUE:
			g_print("disconnecting");
			serialcom_destroy(&fd);
			connected = FALSE;
			break;
	}
}

void m_button_disconnect_clicked(GtkWidget * widget, gpointer data)
{
	g_print("Disconnecting");
	serialcom_destroy(&fd);
	connected = FALSE;
}

void m_entry_serial_port_changed(GtkWidget * widget, gpointer data)
{
	//TODO: Get widget text. Put into a variable, char * serial_port,
	//use serial_port for opening serial port, duh.
	//
}
/************************************************************************/

void m_button_clicked(GtkWidget * widget,  gpointer data)
{
	g_print("Motor say: ");
	switch((int)data) {
		case MOTOR_ON:
			g_print("Vrrrom!\n");
			motor_state = MOTOR_ON;
			serialcom_put(&fd, '1');
			break;
		case MOTOR_OFF:
			g_print("Schreech!\n");
			motor_state = MOTOR_OFF;
			serialcom_put(&fd, '3');
			break;
		default:
			g_print("Error!\n");
			break;
	}
}

volatile char option1;
void m_clicked_option1(GtkWidget * widget, gpointer data)
{
	option1 ^= 1;
	g_print("Here");
	switch(option1) {
		case ON:
			g_print("Option1 on\n");
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id, "Option1 on");
			break;
		case OFF:
			g_print("Option1 off\n");
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id, "Option1 off");
			break;
		default:
			g_print("Strange\n");
	}
}

volatile char option2;
void m_clicked_option2(GtkWidget * widget, gpointer data)
{
	option2 ^= 1;
	switch(option2) {
		case ON:
			g_print("Option2 on\n");
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id, "Option2 on");
			break;
		case OFF:
			g_print("Option2 off\n");
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id, "Option2 off");
			break;
		default:
			g_print("Strange\n");
	}
}

volatile char option3;
void m_clicked_option3(GtkWidget * widget, gpointer data)
{
	option3 ^= 1;
	switch(option3) {
		case ON:
			g_print("Option3 on\n");
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id, "Option3 on");
			break;
		case OFF:
			g_print("Option3 off\n");
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id, "Option3 off");
			break;
		default:
			g_print("Strange\n");
	}
}

void m_ramp_clicked(GtkWidget * widget, gpointer data)
{
	gdouble tmp;
	switch((gint)data) {
		case RAMP_START:
			print_statusbar("Ramp started");
			break;
		case RAMP_STOP:
			print_statusbar("Ramp paused");
			break;
		case RAMP_REVERSE:
			tmp = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button_ramp_from));
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button_ramp_from), gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button_ramp_to)));
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button_ramp_to), tmp);
			print_statusbar("Ramp reversed");
			break;
	}
}

#define STRING_MAX_LEN 0x80
void m_vscale_rpm_value_changed(GtkAdjustment * widget,  gpointer data)
{
	char str[STRING_MAX_LEN];
	//~ snprintf(str, STRING_MAX_LEN, "Setting RPM: %i", (gint)widget->value);
	snprintf(str, STRING_MAX_LEN, "Setting RPM: %i\n", (gint)gtk_range_get_value(GTK_RANGE(widget)));
	print_statusbar(str);
	serialcom_write(&fd, str);
}

int main(int argc, char * argv[])
{
	char ch;

	//~ serialcom_init_old(&fd, "/dev/ttyACM0");

	GtkBuilder * builder;
	GtkWidget * window;
	GtkWidget * togglebutton_connect;
	GtkWidget * button_disconnect;
	GtkWidget * button_status;
	GtkWidget * button_quit;
	GtkWidget * entry_serial_port;

	GtkWidget * togglebutton_motor_start;
	GtkWidget * button_motor_stop;
	GtkWidget * togglebutton_motor_pause;
	GtkWidget * button_reset_mcu;


	GtkWidget * vscale_rpm;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "layout/gui.xml", NULL);
	window = GTK_WIDGET(gtk_builder_get_object (builder, "window"));
	//~ gtk_builder_connect_signals(builder, NULL);
	g_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(on_window_destroy), NULL);

	togglebutton_connect = GTK_WIDGET(gtk_builder_get_object(builder, "togglebutton_connect"));
	g_signal_connect(GTK_OBJECT(togglebutton_connect), "toggled", G_CALLBACK(m_togglebutton_connect_toggled), NULL);

	button_disconnect = GTK_WIDGET(gtk_builder_get_object(builder, "button_disconnect"));
	g_signal_connect(GTK_OBJECT(button_disconnect), "clicked", G_CALLBACK(m_button_disconnect_clicked), NULL);


	entry_serial_port = GTK_WIDGET(gtk_builder_get_object(builder, "entry_serial_port"));
	g_signal_connect(GTK_OBJECT(entry_serial_port), "editing-done", G_CALLBACK(m_entry_serial_port_changed), NULL);


	vscale_rpm = GTK_WIDGET(gtk_builder_get_object(builder, "vscale_rpm"));
	g_signal_connect(GTK_OBJECT(vscale_rpm), "value-changed", G_CALLBACK(m_vscale_rpm_value_changed), NULL);

	button_quit = GTK_WIDGET(gtk_builder_get_object(builder, "button_quit"));
	g_signal_connect(GTK_OBJECT(button_quit), "clicked", G_CALLBACK(m_button_quit), NULL);

	statusbar = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));


	gtk_widget_show(window);

	gtk_main();
	g_object_unref(G_OBJECT(builder));

	serialcom_destroy(&fd);
	return 0;
}

//~
//~ int main_glade(int argc, char ** argv)
//~ {
	//~ GladeXML * xml;
	//~ GtkWidget * widget;
//~
	//~ gtk_init(&argc, &argv);
	//~ xml = glade_xml_new("layout/gui.xml", NULL, NULL);
	//~ widget - glade_xml_get_widget(xml, "window1");
//~
	//~ glade_xml_signal_autoconnect(xml);
//~
	//~ gtk_main();
	//~ return 0;
//~ }
