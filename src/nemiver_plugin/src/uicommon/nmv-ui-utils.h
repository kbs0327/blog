/* -*- Mode: C++; indent-tabs-mode:nil; c-basic-offset:4; -*- */

// Author: Dodji Seketeli

/*
 *This file is part of the Nemiver Project.
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
#ifndef __NMV_UI_UTILS_H__
#define __NMV_UI_UTILS_H__

#include "common/nmv-api-macros.h"
#include <gtkmm.h>
//add my function
#include <vector>
#include <gtksourceviewmm/view.h>
#include <cairomm/context.h>
#include <gtkmm/drawingarea.h>
#include <glibmm/ustring.h>
#include "common/nmv-env.h"
#include "common/nmv-ustring.h"
#include "common/nmv-safe-ptr-utils.h"

#ifndef NEMIVER_CATCH
#define NEMIVER_CATCH \
} catch (Glib::Exception &e) { \
    LOG_ERROR (std::string ("caught exception: '") + e.what () + "'"); \
    nemiver::ui_utils::display_error (e.what ()); \
} catch (std::exception &e) { \
    LOG_ERROR (std::string ("caught exception: '") + e.what () + "'"); \
    nemiver::ui_utils::display_error (e.what ()); \
} catch (...) { \
    LOG_ERROR ("caught unknown exception"); \
    nemiver::ui_utils::display_error ("An unknown error occured"); \
}
#endif

#ifndef NEMIVER_CATCH_AND_RETURN
#define NEMIVER_CATCH_AND_RETURN(a_value) \
} catch (Glib::Exception &e) { \
    LOG_ERROR (std::string ("caught exception: '") + e.what () + "'"); \
    nemiver::ui_utils::display_error (e.what ()); \
    return a_value; \
} catch (std::exception &e) { \
    LOG_ERROR (std::string ("caught exception: '") + e.what () + "'"); \
    nemiver::ui_utils::display_error (e.what ()); \
    return a_value; \
} catch (...) { \
    LOG_ERROR ("Caught unknown exception"); \
    nemiver::ui_utils::display_error ("An unknown error occured"); \
    return a_value; \
}
#endif

using nemiver::common::UString;

namespace nemiver {
namespace ui_utils {

class ActionEntry {

public:
    enum Type {
        DEFAULT=0,
        TOGGLE
    };

    common::UString m_name;
    Gtk::StockID m_stock_id;
    common::UString m_label;
    common::UString m_tooltip;
    sigc::slot<void> m_activate_slot;
    Type m_type;
    common::UString m_accel;
    bool m_is_important;

    Glib::RefPtr<Gtk::Action> to_action () const
    {
        Glib::RefPtr<Gtk::Action> result;
        switch (m_type) {
            case DEFAULT:
                if (m_stock_id.get_string () != "") {
                    result =
                        Gtk::Action::create (m_name, m_stock_id,
                                             m_label, m_tooltip);
                } else {
                    result =
                        Gtk::Action::create (m_name, m_label, m_tooltip);
                }
                break;
            case TOGGLE:
                if (m_stock_id.get_string () != "") {
                    result =
                        Gtk::ToggleAction::create (m_name, m_stock_id,
                                                   m_label, m_tooltip);
                } else {
                    result =
                        Gtk::ToggleAction::create (m_name,
                                                   m_label,
                                                   m_tooltip);
                }
                break;

            default:
                THROW ("should never reach this point");
        }

        if (result)
            result->set_is_important (m_is_important);

        return result;
    }
};//end class ActionEntry

NEMIVER_API void add_action_entries_to_action_group
                                (const ActionEntry a_tab[],
                                 int a_num_entries,
                                 Glib::RefPtr<Gtk::ActionGroup> &a_group);

NEMIVER_API int display_info (const common::UString &a_message);

NEMIVER_API int display_warning (const common::UString &a_message);

NEMIVER_API int display_error (const common::UString &a_message);

NEMIVER_API int ask_yes_no_question (const common::UString &a_message);

NEMIVER_API int ask_yes_no_question (const common::UString &a_message,
                                     bool a_propose_dont_ask_question,
                                     bool &a_dont_ask_this_again);

NEMIVER_API int ask_yes_no_cancel_question (const common::UString &a_message);

NEMIVER_API bool ask_user_to_select_file (const UString &a_file_name,
                                          const UString &a_default_dir,
                                          UString &a_selected_file_path);

NEMIVER_API bool find_file_or_ask_user (const UString& a_file_path,
                                        const list<UString> &a_where_to_look,
                                        list<UString> &a_session_dirs,
                                        map<UString, bool> &a_ignore_paths,
                                        bool a_ignore_if_not_found,
                                        UString& a_absolute_path);


bool find_file_and_read_line (const UString &a_file_path,
                              const list<UString> &a_where_to_look,
                              list<UString> &a_sess_dirs,
                              map<UString, bool> &a_ignore_paths,
                              int a_line_number,
                              string &a_line);

template <class T>
T*
get_widget_from_gtkbuilder (const Glib::RefPtr<Gtk::Builder> &a_gtkbuilder,
                            const UString &a_widget_name)
{
    T *widget;
    a_gtkbuilder->get_widget (a_widget_name, widget);
    if (!widget) {
        THROW ("couldn't find widget '"
               + a_widget_name);
    }
    return widget;
}


struct WidgetRef {
    void operator () (Gtk::Widget *a_widget)
    {
        if (a_widget) {
            a_widget->reference ();
        }
    }
};//end struct WidgetRef

struct WidgetUnref {
    void operator () (Gtk::Widget *a_widget)
    {
        if (a_widget) {
            a_widget->unreference ();
        }
    }
};//end struct WidgetUnref
}//end namespace ui_utils

#ifndef _MyFunction_H
#define _MyFunction_H

class MyValue
{
private:
    UString name;
    UString value;
    UString type;
public:
    MyValue();
    MyValue(UString n, UString v, UString t);
    void setName(UString n);
    void setValue(UString v);
    void setType(UString t);
    UString getName();
    UString getValue();
    UString getType();
};

struct RGBA
{
    double r;
    double g;
    double b;
    double a;
};

class Figure
{
protected:
    double x;
    double y;
    struct RGBA rgba;
    static Cairo::RefPtr<Cairo::Context> cr;
    static Gtk::Widget* widget;
    static bool drawable;


public:
    Figure();
    //Figure(const Cairo::RefPtr<Cairo::Context> &_cr, Gtk::Widget* w);
    ~Figure();
    double getX();
    double getY();
    static void setWidget(Gtk::Widget* w);
    static void setCairo(const Cairo::RefPtr<Cairo::Context> &_cr);
    static Gtk::Widget* getWidget();
    void setX(double _x);
    void setY(double _y);
    void setCoordinate(double _x, double _y);
    void setRGBA(double r, double g, double b, double a = 1);
    void setColor();
    void setColor(double r, double g, double b, double a=1);
    void draw();
};

class MyFunction : public Figure
{
 private :
    //data
    UString funcName;
    UString id;
    int call_num;
    UString file_path;
    UString file_full_path;
    int line_no;
   // int end_line_no;
    vector<MyValue*> arguments;
    vector<MyValue*> local_values;

    //draw
    vector<MyFunction*> call_func;
    double width;
    double height;
    const static double ch_h=30;        //글자 간격
    const static double line_thick=2.0;
    const static double between_functions_width_distance = 100;
    const static double between_functions_height_distance = 20;


    //draw function
    void drawRowLine(int s_b_num, double l_t = line_thick);      //st_y : 상자 시작 기준 거리
    void drawColLine(double st_x, int s_b_num, int b_num, double l_t = line_thick);
    void drawTitle(UString& str, int d_y);
    void drawValueStr(UString& str, int d_x, int d_y, int d_w);
    void drawText(UString& str, int d_x, int d_y, int d_w, Pango::Weight w, int font_size);
    void drawOutline();
    void drawValues();
    void drawLine(double to_x, double to_y);

        
public:
    //data
    MyFunction();
    MyFunction(UString _id);
    MyFunction(UString _id, UString _f_n);
    MyFunction(UString _f_n, int as);
    MyFunction(UString _id, UString _f_n, int _c_n);
    //MyFunction(const Cairo::RefPtr<Cairo::Context> &_cr, Gtk::Widget* _w);
    ~MyFunction();
    int calcHeight();   //calculate height and return block num;
    void setFuncName(UString f_n);
    void setID(UString _id);
    void setCallNum(int _c_n);
    //void setFileName(UString file_name);
    //void setLineNo(int l_n);
    void addArg(MyValue* v);
    void addArg(UString name, UString value, UString type);
    void addLocalVal(MyValue* v);
    void addLocalVal(UString name, UString value, UString type);
    void editArg(UString name, UString value, UString type);
    void editLocalVal(UString name, UString value, UString type);
    void setFilePath(UString& path);
    void setFileFullPath(UString& path);
    void setLineNo(int no);
    UString getFuncName();
    UString getID();
    //UString getFileName();
    //int getStLineNo();
    //int getCurLineNo();
    int getCallNum();
    int getArgSize();
    int getLocalValSize();
    MyValue* getArg(int i);
    MyValue* getArg(UString n);
    MyValue* getLocalVal(int i);
    MyValue* getLocalVal(UString n);
    UString& getFilePath();
    UString& getFileFullPath();
    int getLineNo();
    //deal with call function
    void removeCallFunction();
    void addCallFunc(MyFunction* c);
    MyFunction* findChild(double _x, double _y);
    MyFunction* find(double _x, double _y);

    //draw
    void setFocus();
    void releaseFocus();
    void setWidth(double w);
    void setHeight(double h);
    void setW_H(double w, double h);
    double getWidth();
    double getHeight();
    void draw();
    double draw(double _x, double _y, double& max_height);

      

};

struct MyCallFunc
{
    MyFunction * call_func;
    bool is_function_call; //함수가 불린 것인지 확인(true : 함수 호출, false : 함수 반환)
};

class IPerspective;
class MyFunctionManager
{
private:
    map<UString, MyFunction*> func_factory;
    map<UString, int*> func_call_num;
    vector<struct MyCallFunc*> call_sequence;
    UString prev_func_name;
    UString id;
    MyFunction* deal_with_func;
    double win_x;
    double win_y;
    bool func_name_check;
    bool addable;
    bool editable;
    bool is_new_frame;
    //static MyFunctionManager func_manager;
    static UString ID_SEPERATOR;
    int max_depth;
    int depth; UString first_file_path;
    UString first_file_full_path;
    int first_line_no;

    //sigc::signal<void> m_restart_and_draw_signal();
    //redraw variable
    IPerspective *dbg_persp;
    sigc::signal<void> m_restart_signal;
    bool is_drawing; //true : draw, false : don't draw
    bool is_continue; //simulation
    bool is_to_bp;
    UString target_file_path;
    UString target_file_full_path;
    int target_line_no;
    int target_call_no;

    MyFunctionManager();
    bool isBreakPoint(UString& f_full_path, UString& f_path, int l_no);
    bool setBreakPoint(UString& f_full_path, UString& f_path, int l_no, bool is_set);
    bool isSameFunction();
    bool existFunction();
    bool existFunction(UString str);
    bool isParentFunction(UString f_n);
    bool isTarget(UString& f_full_path, UString& f_path, int l_no);
    void start_simulation(UString& f_full_path, UString& f_path, int l_no);
    void do_simulation(UString& f_full_path, UString& f_path, int l_no);
    void do_quickContinue(UString& f_full_path, UString& f_path, int l_no);
    UString expandID(UString f_n);
    void reduceID();
    int addFuncCall(UString f_n);
    MyFunction* findHitFunction(double _x, double _y);

public:
    int oldframenum;
    static MyFunctionManager* getFuncManager()
    {
        /*if(func_manager == NULL)
        {
            func_manager = new MyFunctionManager();
        }
        return func_manager;*/
        static MyFunctionManager func_manager;
        return &func_manager;
    }
    void clear();
    bool isDrawing();
    void setFuncName(UString f_n, bool is_n_f, UString f_full_path, UString  f_path, int l_no); //반드시 모든 작업 앞에 수행되야됨
    void setAddable();
    void setEditable();
    void endDealWithFunction();
    //void setFileName(UString file_name);
   // void setLineNo(int l_n);
   // void setLevel(int l);
    void addArg(UString name, UString value, UString type);
    void addLocalVal(UString name, UString value, UString type);
    void editArg(UString name, UString value, UString type);
    void editLocalVal(UString name, UString value, UString type);
    void setRetVal(UString value, UString type);
    void onClicked(double _x, double _y);
    int removeFunction(UString id);
     UString getFuncName();
    // int getLevel();
     //UString getFileName();
     //int getCurLineNo();
     int getFuncCallNum(UString f_n);
     int getFuncCallNum();
     int getArgSize();
     int getLocalValSize();
     double getWinX();
     double getWinY();
     MyValue* getArg(int i);
     MyValue* getArg(UString key);
     MyValue* getVal(int i);
     MyValue* getVal(UString key);
     //redraw function
     sigc::signal<void>& getRestartSignal();
     void setDBGPersp(IPerspective& dbg);
     void restartWithDrawing();
     void restartWithDrawingToTarget();
     void restartQuickly();
     void goToBp();


     void draw(int w, int h);

     struct MyCallFunc* getCallFuncAt(int i);
};





class Progress_MyFunction : public Figure
{
 private :
    //data
    UString funcName;
    //draw
  
    double progress_width;
    double progress_height;
    const static double ch_h=30;        //글자 간격
    const static double line_thick=2.0;
    const static double progress_line_thick=17.0;
    const static double between_functions_width_distance = 100;
    const static double between_functions_height_distance = 20;


    //draw function
    void drawRowLine(int s_b_num, double l_t = line_thick);      //st_y : 상자 시작 기준 거리
    void drawTitle(UString& str, int d_y);
    void drawText(UString& str, int d_x, int d_y, int d_w, Pango::Weight w, int font_size);
    void drawOutline();
    void drawLine(double to_x, double to_y);
    void setWidth(double w);

        
public:
    //data
    Progress_MyFunction();
    Progress_MyFunction(UString _f_n, int as);
    ~Progress_MyFunction();
    UString getFuncName();
    int getArgSize();

    //draw
    void draw();
    double draw(double _x, double _y, double width);
    void progress_drawOutline();
    void progress_drawRowLine(int s_b_num, double l_t = progress_line_thick);
    void progress_drawTitle(UString& str,int d_x, int d_y);
      

};


class Prgoress_MyFunctionManager
{
private:
   
    vector<Progress_MyFunction*> progressbar;
    vector<int *> progressbar_num;
    double win_x;
    double win_y;
    
  
    int progressbar_chage;

    Prgoress_MyFunctionManager();
  
  

public:
    int oldframenum;
    static Prgoress_MyFunctionManager* getFuncManager()
    {
        /*if(func_manager == NULL)
        {
            func_manager = new MyFunctionManager();
        }
        return func_manager;*/
        static Prgoress_MyFunctionManager func_manager;
        return &func_manager;
    }  
     void draw(int w, int h);
     void progressbar_setFuncName(UString f_n, int framenum);
     double getWinX();
     double getWinY();
     void clear();

};
#endif
}//end namespace nemiver
#endif// __NMV_UI_UTILS_H__
