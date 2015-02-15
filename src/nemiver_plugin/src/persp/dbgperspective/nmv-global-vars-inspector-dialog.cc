//Author: Dodji Seketeli
/*
 *This file is part of the Nemiver project
 *
 *Nemiver is free software; you can redistribute
 *it and/or modify it under the terms of
 *the GNU General Public License as published by the
 *Free Software Foundation; either version 2,
 *or (at your option) any later version.
 *
 *Nemiver is distributed in the hope that it will
 *be useful, but WITHOUT ANY WARRANTY;
 *without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *See the GNU General Public License for more details.
 *
 *You should have received a copy of the
 *GNU General Public License along with Nemiver;
 *see the file COPYING.
 *If not, write to the Free Software Foundation,
 *Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *See COPYRIGHT file copyright information.
 */
#include "config.h"

#include <map>
#include <list>
#include <glib/gi18n.h>
#include "common/nmv-exception.h"
#include "nmv-global-vars-inspector-dialog.h"
#include "nmv-variables-utils.h"
#include "nmv-vars-treeview.h"
#include "nmv-ui-utils.h"
#include "nmv-i-workbench.h"
#include "nmv-i-var-list-walker.h"
#include "nmv-debugger-utils.h"

using namespace nemiver::common;
namespace vutil = nemiver::variables_utils2;

NEMIVER_BEGIN_NAMESPACE (nemiver)

struct GlobalVarsInspectorDialog::Priv : public sigc::trackable {
private:
    Priv ();
public:
    Gtk::Dialog &dialog;
    Glib::RefPtr<Gtk::Builder> gtkbuilder;
    IDebuggerSafePtr debugger;
    IVarListWalkerSafePtr global_variables_walker_list;

    IWorkbench &workbench;
    Gtk::Widget &tree_view;
    Glib::RefPtr<Gtk::TreeStore> tree_store;
    Gtk::TreeModel::iterator cur_selected_row;
    SafePtr<Gtk::Menu> contextual_menu;
    UString previous_function_name;
	Gtk::Button *close_button;

	Gtk:: ScrolledWindow *scr;


    Priv (Gtk::Dialog &a_dialog,
          const Glib::RefPtr<Gtk::Builder> &a_gtkbuilder,
          IDebuggerSafePtr &a_debugger,
          IWorkbench &a_workbench,Gtk::Widget &tree_view2) :
        dialog (a_dialog),
        gtkbuilder (a_gtkbuilder),
        workbench (a_workbench),
        tree_view (tree_view2)
    {
        LOG_FUNCTION_SCOPE_NORMAL_DD;

        THROW_IF_FAIL (a_debugger);

        debugger = a_debugger;
       // build_tree_view ();
       // re_init_tree_view ();
       // connect_to_debugger_signals ();
       // init_graphical_signals ();
        build_dialog ();
       // debugger->list_global_variables ();
    }

    void build_dialog ()
    {

		if(close_button==NULL)
		{close_button =
           ui_utils::get_widget_from_gtkbuilder<Gtk::Button> (gtkbuilder,
                                                                            "closebutton");
           THROW_IF_FAIL (close_button);
           close_button->set_sensitive (true);
           close_button->signal_clicked ().connect
	           (sigc::mem_fun (*this, &Priv::on_close_button_clicked_signal));
			Gtk::Box *box  =
                ui_utils::get_widget_from_gtkbuilder<Gtk::Box> (gtkbuilder,
                                                               "inspectorwidgetbox");
           THROW_IF_FAIL (box);
		
           Gtk:: ScrolledWindow *scr = Gtk::manage (new Gtk::ScrolledWindow);
               THROW_IF_FAIL (scr);
           scr->set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
           scr->set_shadow_type (Gtk::SHADOW_IN);
                  //changed...

			      //tree_view.show_all();
				 
            scr->add (tree_view);
		    tree_view.set_size_request (500, 500);
            box->pack_start (*scr);
		    scr->set_size_request (500, 500);
				  //box->show_all ();

		}	
		  //   widget().show_all();
		else {
		Gtk::Box *box =
           ui_utils::get_widget_from_gtkbuilder<Gtk::Box> (gtkbuilder,
                                                               "inspectorwidgetbox");
			scr->remove();
			scr->add (tree_view);
			box->pack_start (*scr);
			box->show_all ();
			dialog.show();
		}
	    dialog.show_all ();
    }
	void on_close_button_clicked_signal(){

        dialog.hide();
		

   }

};//end GlobalVarsInspectorDialog::Priv

GlobalVarsInspectorDialog::GlobalVarsInspectorDialog
                                            (const UString &a_root_path,
        IDebuggerSafePtr &a_debugger, IWorkbench &a_workbench, Gtk::Widget &tree_view) : Dialog (a_root_path,
            "globalvarsinspector.ui",
            "globalvarsinspector")
{   
	
    m_priv.reset (new Priv (widget (), gtkbuilder (), a_debugger, a_workbench, tree_view));
}

GlobalVarsInspectorDialog::~GlobalVarsInspectorDialog ()
{
    LOG_D ("deleted", "destructor-domain");
}
/*
static GlobalVarsInspectorDialog *getDialog()
{
	static GlobalVarsInspectorDialog dialog;
	return &dialog;
}
*/

NEMIVER_END_NAMESPACE (nemiver)

