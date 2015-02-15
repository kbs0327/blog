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
#include "config.h"
#include <glib/gi18n.h>
#include <cairomm/context.h>
#include <gtkmm/drawingarea.h>
//#include <glibmm/ustring.h>
#include <cmath>
#include <cstring>
#include "common/nmv-exception.h"
#include "nmv-ui-utils.h"
#include "nmv-locate-file-dialog.h"
#include "../persp/nmv-i-perspective.h"


using namespace nemiver::common;

namespace nemiver {
namespace ui_utils {

void
add_action_entries_to_action_group (const ActionEntry a_tab[],
                                    int a_num_entries,
                                    Glib::RefPtr<Gtk::ActionGroup> &a_group)
{
    THROW_IF_FAIL (a_group);

    for (int i=0; i < a_num_entries; ++i) {
        Glib::RefPtr<Gtk::Action> action = a_tab[i].to_action ();
        if (a_tab[i].m_accel != "") {
            a_group->add (action,
                          Gtk::AccelKey (a_tab[i].m_accel),
                          a_tab[i].m_activate_slot);
        } else {
            a_group->add (action, a_tab[i].m_activate_slot);
        }
    }
}

/// A message dialog that allows the user to chose to not
/// be shown that dialog again
class DontShowAgainMsgDialog : public Gtk::MessageDialog {

    DontShowAgainMsgDialog (const DontShowAgainMsgDialog&);
    DontShowAgainMsgDialog& operator= (const DontShowAgainMsgDialog&);

    Gtk::CheckButton *m_check_button;
public:
    explicit DontShowAgainMsgDialog (const Glib::ustring &a_message,
                                     bool a_propose_dont_ask_again = false,
                                     bool a_use_markup = false,
                                     Gtk::MessageType a_type
                                                         = Gtk::MESSAGE_INFO,
                                     Gtk::ButtonsType a_buttons
                                                         = Gtk::BUTTONS_OK,
                                     bool a_modal = false) :
        Gtk::MessageDialog (a_message,
                            a_use_markup,
                            a_type,
                            a_buttons,
                            a_modal),
        m_check_button (0)
    {
        if (a_propose_dont_ask_again)
            pack_dont_ask_me_again_question ();
    }

    void pack_dont_ask_me_again_question ()
    {
        RETURN_IF_FAIL (!m_check_button);

        m_check_button =
            Gtk::manage (new Gtk::CheckButton (_("Do not ask me again")));
        RETURN_IF_FAIL (m_check_button);

        Gtk::Alignment *align =
            Gtk::manage (new Gtk::Alignment);
        RETURN_IF_FAIL (align);

        align->add (*m_check_button);
        RETURN_IF_FAIL (get_vbox ());

        align->show_all ();
        get_vbox ()->pack_end (*align, true, true, 6);
    }

    bool dont_ask_this_again () const
    {
        if (!m_check_button)
            return false;
        return m_check_button->get_active ();
    }
};//end class DontShowAgainMsgDialog

int
display_info (const UString &a_message)
{
    Gtk::MessageDialog dialog (a_message, false, Gtk::MESSAGE_INFO,
                                               Gtk::BUTTONS_OK, true);
    dialog.set_default_response (Gtk::RESPONSE_OK);
    return dialog.run ();
}

int
display_warning (const UString &a_message)
{
    Gtk::MessageDialog dialog (a_message, false,
                               Gtk::MESSAGE_WARNING,
                               Gtk::BUTTONS_OK, true);
    dialog.set_default_response (Gtk::RESPONSE_OK);
    return dialog.run ();
}

int
display_error (const UString &a_message)
{
    Gtk::MessageDialog dialog (a_message, false,
                               Gtk::MESSAGE_ERROR,
                               Gtk::BUTTONS_OK, true);
    dialog.set_default_response (Gtk::RESPONSE_OK);
    return dialog.run ();
}

int
ask_yes_no_question (const UString &a_message)
{
    Gtk::MessageDialog dialog (a_message, false,
                               Gtk::MESSAGE_QUESTION,
                               Gtk::BUTTONS_YES_NO, true);
    dialog.set_default_response (Gtk::RESPONSE_OK);
    return dialog.run ();
}

int
ask_yes_no_question (const UString &a_message,
                     bool a_propose_dont_ask_again,
                     bool &a_dont_ask_this_again)
{
    DontShowAgainMsgDialog dialog (a_message, a_propose_dont_ask_again,
                                   false, Gtk::MESSAGE_QUESTION,
                                   Gtk::BUTTONS_YES_NO, true);
    dialog.set_default_response (Gtk::RESPONSE_OK);
    int result = dialog.run ();
    a_dont_ask_this_again = dialog.dont_ask_this_again ();
    return result;
}

int
ask_yes_no_cancel_question (const common::UString &a_message)
{
    Gtk::MessageDialog dialog (a_message, false,
                               Gtk::MESSAGE_QUESTION,
                               Gtk::BUTTONS_NONE,
                               true);

    dialog.add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button (Gtk::Stock::NO, Gtk::RESPONSE_NO);
    dialog.add_button (Gtk::Stock::YES, Gtk::RESPONSE_YES);
    dialog.set_default_response (Gtk::RESPONSE_CANCEL);
    return dialog.run ();
}

bool
ask_user_to_select_file (const UString &a_file_name,
                         const UString &a_default_dir,
                         UString &a_selected_file_path)
{
    LocateFileDialog dialog ("", a_file_name);
    // start looking in the default directory
    dialog.file_location (a_default_dir);
    int result = dialog.run ();
    if (result == Gtk::RESPONSE_OK) {
        UString file_path = dialog.file_location ();
        if (!Glib::file_test (file_path, Glib::FILE_TEST_IS_REGULAR)
            || (Glib::path_get_basename (a_file_name)
                != Glib::path_get_basename (file_path)))
            return false;
        UString parent_dir =
            Glib::filename_to_utf8 (Glib::path_get_dirname
                                                (dialog.file_location ()));
        if (!Glib::file_test (parent_dir, Glib::FILE_TEST_IS_DIR))
            return false;

        a_selected_file_path = file_path;
        return true;
    }
    return false;
}

bool
find_file_or_ask_user (const UString& a_file_name,
                       const list<UString> &a_where_to_look,
                       list<UString> &a_session_dirs,
                       map<UString, bool> &a_ignore_paths,
                       bool a_ignore_if_not_found,
                       UString& a_absolute_path)
{
    if (!env::find_file (a_file_name, a_where_to_look, a_absolute_path)) {
        if (a_ignore_paths.find (a_file_name)
            != a_ignore_paths.end ())
            // We didn't find a_file_name but as we were previously
            // requested to *not* ask the user to locate it, just
            // pretend we didn't find the file.
            return false;
        if (ask_user_to_select_file (a_file_name,
                                     a_where_to_look.front (),
                                     a_absolute_path)) {
            UString parent_dir =
                Glib::filename_to_utf8 (Glib::path_get_dirname
                                        (a_absolute_path));
            a_session_dirs.push_back (parent_dir);
        } else {
            if (a_ignore_if_not_found)
                // Don't ask the user to locate a_file_path next time.
                a_ignore_paths[a_file_name] = true;
            return false;
        }
    }
    return true;
}

/// Given a file path P and a line number N , reads the line N from P
/// and return it iff the function returns true. This is useful
/// e.g. when forging a mixed source/assembly source view, and we want
/// to display a source line N from a file P.
///
/// \param a_file_path the file path to consider. Not necessarily
/// absolute. If the file is not found, it will be searched for very hard
/// in many places.
/// \param a_where_to_look the list of directories where to look for files.
/// \param a_sess_dirs if the user helps to find the a file,
/// the directory given by the user will be added to this list.
/// \param a_ignore_paths a list of file paths to not search for. If
/// a_file_path is present in these, we will not search for it.
/// \param a_ignore_if_not_found if true and if we haven't found
/// a_file_path so far, then add it to a_ignore_paths.
/// \param a_line_number the line number to consider
/// \param a_line the string containing the resulting line read, if
/// and only if the function returned true.
/// \return true upon successful completion, false otherwise.
bool
find_file_and_read_line (const UString &a_file_path,
                         const list<UString> &a_where_to_look,
                         list<UString> &a_sess_dirs,
                         map<UString, bool> &a_ignore_paths,
                         int a_line_number,
                         string &a_line)
{
    if (a_file_path.empty ())
        return false;

    UString path;
    if (!find_file_or_ask_user (a_file_path,
                                a_where_to_look,
                                a_sess_dirs,
                                a_ignore_paths,
                                true, path))
        return false;

    return env::read_file_line (path, a_line_number, a_line);
}
}//end namespace ui_utils



typedef pair<UString, MyFunction*> Str_Func_Pair;
typedef pair<map<UString, MyFunction*>::iterator, bool> Itr_Func_Pair;
typedef map<UString, MyFunction*>::iterator Str_Func_Itr;
typedef pair<UString, int*> Str_Int_Pair;
typedef pair<map<UString, int*>::iterator, bool> Itr_Int_Pair;
typedef map<UString, int*>::iterator Str_Int_Itr;

MyValue::MyValue()
{
    name = "";
    value = "";
    type = "";
}

MyValue::MyValue(UString n, UString v, UString t) : name(n), value(v), type(t)
{

}

void MyValue::setName(UString n)
{
    name = n;
}

void MyValue::setValue(UString v)
{
    value = v;
}

void MyValue::setType(UString t)
{
    type = t;
}

UString MyValue::getName()
{
    return name;
}

UString MyValue::getValue()
{
    return value;
}

UString MyValue::getType()
{
    return type;
}

Figure::Figure()// : cr(NULL), widget(NULL)
{
    x = 0;
    y = 0;
    rgba.r = 0;
    rgba.g = 0;
    rgba.b = 0;
    rgba.a = 1;
   // drawable = false;
}

/*Figure::Figure(const Cairo::RefPtr<Cairo::Context> &_cr, Gtk::Widget* w) : cr(_cr), widget(w)
{
    x = 0;
    y = 0;
    rgba.r = 0;
    rgba.g = 0;
    rgba.b = 0;
    rgba.a = 1;
    drawable = false;
}*/

Cairo::RefPtr<Cairo::Context> Figure::cr(NULL);
Gtk::Widget* Figure::widget =NULL;
bool Figure::drawable = false;

Figure::~Figure()
{

}

double Figure::getX()
{
    return x;
}

double Figure::getY()
{
    return y;
}

void Figure::setWidget(Gtk::Widget* w)
{
    widget = w;
    if(!cr)
        drawable = true;
}

void Figure::setCairo(const Cairo::RefPtr<Cairo::Context> &_cr)
{
    cr = _cr;
    if(!widget)
        drawable = true;
}

Gtk::Widget* Figure::getWidget()
{
    return widget;
}

void Figure::setX(double _x)
{
    x = _x;
}

void Figure::setY(double _y)
{
    y = _y;
}

void Figure::setCoordinate(double _x, double _y)
{
    x = _x;
    y = _y;
}

void Figure::setRGBA(double r, double g, double b, double a)
{
    rgba.r = r;
    rgba.g = g;
    rgba.b = b;
    rgba.a = a;
}

void Figure::setColor()
{
    cr->set_source_rgba(rgba.r,rgba.g,rgba.b,rgba.a);
}

void Figure::setColor(double r, double g, double b, double a)
{
    cr->set_source_rgba(r,g,b,a);
}



MyFunction::MyFunction() : Figure()
{
    funcName = "";
    id = "";
    arguments.clear();
    local_values.clear();
    call_num = 0;
    width = 300;
    height = 7*ch_h;
    call_func.clear();
    setFocus();
}

MyFunction::MyFunction(UString _f_n, int as) : Figure(), funcName(_f_n)
{

    id = "";
    arguments.clear();
    local_values.clear();
    call_num = 0;
    width = 300;
    height = 7*ch_h;
    call_func.clear();
    setRGBA(0.7,0.9,0.9);
}

MyFunction::MyFunction(UString _id) : Figure(), id(_id)
{
    funcName = "";
    arguments.clear();
    local_values.clear();
    call_num = 0;
    width = 300;
    height = 7*ch_h;
    call_func.clear();
    setFocus();
}

MyFunction::MyFunction(UString _id, UString _f_n) : Figure(), id(_id), funcName(_f_n)
{
    arguments.clear();
    local_values.clear();
    call_num = 0;
    width = 300;
    height = 7*ch_h;
    call_func.clear();
    setFocus();
}

MyFunction::MyFunction(UString _id, UString _f_n, int _c_n) :Figure(), id(_id), funcName(_f_n), call_num(_c_n)
{
    arguments.clear();
    local_values.clear();
    width = 300;
    height = 7*ch_h;
    call_func.clear();
    setFocus();
}

/*MyFunction::MyFunction(const Cairo::RefPtr<Cairo::Context> &_cr, Gtk::Widget* _w) : Figure(_cr, _w)
{
    funcName = "";
    id = "";
    arguments.clear();
    local_values.clear();
    return_value = NULL;
    call_num = 0;
    width = 300;
    height = 7*ch_h;
}*/

MyFunction::~MyFunction()
{

   for(int i = 0; i <arguments.size() ; i++)
   {
       delete arguments.at(i);
   }

   for(int i = 0; i <local_values.size() ; i++)
   {
       delete local_values.at(i);
   }
}

void MyFunction::setFuncName(UString f_n)
{
    funcName = f_n;
}

void MyFunction::setID(UString _id)
{
    id = _id;
}

void MyFunction::setCallNum(int _c_n)
{
    call_num = _c_n;
}
/*
void MyFunction::setFileName(UString file_name)
{
    fileName = file_name;
}

void MyFunction::setLineNo(int l_n)
{
    if(!st_line_no)
        st_line_no = l_n;
    end_line_no = l_n;
}
*/
void MyFunction::addArg(MyValue* v)
{
    arguments.push_back(v);
}

void MyFunction::addArg(UString name, UString value, UString type)
{
    for(int i = 0; i < arguments.size(); i++)
    {
        if(arguments[i]->getName() == name)
        {
            editArg(name, value, type);
            return;
        }
    }
    MyValue* v = new MyValue(name, value, type);
    addArg(v);
}

void MyFunction::addLocalVal(MyValue* v)
{
    local_values.push_back(v);
}

void MyFunction::addLocalVal(UString name, UString value, UString type)
{
    for(int i = 0; i < local_values.size(); i++)
    {
        if(local_values[i]->getName() == name)
        {
            editLocalVal(name, value, type);
            return;
        }
    }
    MyValue* v = new MyValue(name, value, type);
    addLocalVal(v);
}

void MyFunction::editArg(UString name, UString value, UString type)
{
    for(int i = 0; i <arguments.size() ; i++)
    {
        if(arguments.at(i)->getName() == name)
        {
            MyValue* v = arguments.at(i);
            v->setValue(value);
            v->setType(type);
            return;
        }
    }

    LOG_DD("arguments don't have this variable");
}

void MyFunction::editLocalVal(UString name, UString value, UString type)
{
    for(int i = 0; i <local_values.size() ; i++)
    {
        if(local_values.at(i)->getName() == name)
        {
            MyValue* v = local_values.at(i);
            v->setValue(value);
            v->setType(type);
            return;
        }
    }

    LOG_DD("arguments don't have this variable");
}
void MyFunction::setFilePath(UString& path)
{
    file_path = path;
}

void MyFunction::setFileFullPath(UString& path)
{
    file_full_path = path;
}

void MyFunction::setLineNo(int no)
{
    line_no =no;
}


UString MyFunction::getFuncName()
{
    return funcName;
}

UString MyFunction::getID()
{
    return id;
}

/*UString MyFunction::getFileName()
{
    return fileName;
}

 int MyFunction::getStLineNo()
 {
     return st_line_no;
 }

int MyFunction::getCurLineNo()
{
    return end_line_no;
}*/

 int MyFunction::getCallNum()
 {
     return call_num;
 }

int MyFunction::getArgSize()
{
    return arguments.size();
}

int MyFunction::getLocalValSize()
{
    return local_values.size();
}

MyValue* MyFunction::getArg(int i)
{
    if(i >= arguments.size())
        LOG_DD("i is over than arguments size");
    return arguments.at(i);
}

MyValue* MyFunction::getArg(UString n)
{
    for(int i = 0; i < arguments.size(); i++)
    {
        if(arguments.at(i)->getName() == n)
        {
            return arguments.at(i);
        }
    }
    LOG_DD("n does not exist on arguments");
    return NULL;
}

MyValue* MyFunction::getLocalVal(int i)
{
    if(i >= local_values.size())
        LOG_DD("i is over than local values size");
    return local_values.at(i);
}

MyValue* MyFunction::getLocalVal(UString n)
{
    for(int i = 0; i < local_values.size(); i++)
    {
        if(local_values.at(i)->getName() == n)
        {
            return local_values.at(i);
        }
    }
    LOG_DD("n does not exist on local values");
    return NULL;
}



UString& MyFunction::getFilePath()
{
    return file_path;
}

UString& MyFunction::getFileFullPath()
{
    return file_full_path;
}

int MyFunction::getLineNo()
{
    return line_no;
}


int MyFunction::calcHeight()
{
    int h = 3;
    h += getArgSize();
    h += getLocalValSize();
    setHeight(h * ch_h);
    return h;
}
void MyFunction::removeCallFunction()
{
    int size =call_func.size();
    MyFunctionManager* func_mngr = MyFunctionManager::getFuncManager();
    for(int i = 0; i < size; i++)
    {
          if(func_mngr->removeFunction(call_func[i]->getID()))
        delete call_func[i];
    }
    call_func.clear();
}

void MyFunction::addCallFunc(MyFunction* c)
{
    call_func.push_back(c);
}
MyFunction* MyFunction::find(double _x, double _y)
{
    if(y>_y)
        return NULL;

    if(x > _x)
        return NULL;

    if(((y + height) >= _y) && ((x+width) >= _x))
    {
         return this;
    }
    return this->findChild(_x,_y);
}

MyFunction* MyFunction::findChild(double _x, double _y)
{
    int sz = call_func.size();
    for(int i = sz - 1; i >= 0; i--)
    {
        if(call_func[i]->getY() <= _y)
        {
            double l_y = call_func[i]->getY();
            double l_h = call_func[i]->getHeight();
            if((l_y+l_h) >= _y)
            {
                double l_x = call_func[i]->getX();
                double l_w = call_func[i]->getWidth();
                if((l_x+l_w) >= _x)
                {
                    if(l_x <= _x)
                    {
                        return call_func[i];
                    }
                    else
                    {
                        return NULL;
                    }
                }
                else
                {
                    return call_func[i]->findChild(_x, _y);
                }
            }
            else
            {
                return call_func[i]->findChild(_x, _y);
            }
        }
    }
    return NULL;
}

void MyFunction::setFocus()
{
    setRGBA(0.7,0.9,0.9);
}

void MyFunction::releaseFocus()
{
    setRGBA(0.9,0.9,0.7);
}
void MyFunction::setWidth(double w)
{
    width = w;
}


void MyFunction::setHeight(double h)
{
    height = h;
}


void MyFunction::setW_H(double w, double h)
{
    width = w;
    height = h;
}


double MyFunction::getWidth()
{
    return width;
}

double MyFunction::getHeight()
{
    return height;
}


void MyFunction::draw()
{
    if(!drawable)
        LOG_DD("can't draw function");

    if(width==0||height==0)
        LOG_DD ("no drawing");

    if((rgba.r ==0 && rgba.g==0 && rgba.b==0)||rgba.a==0)
        LOG_DD ("white image");

    drawOutline();

    drawValues();
}


double MyFunction::draw(double _x, double _y, double& max_height)
{
    calcHeight();
    double c_x = _x + width + between_functions_width_distance;
    double c_y = _y;
    double s_y;
    int sz = call_func.size();
    setCoordinate(_x, _y);
    for(int i = 0; i < sz; i++)
    {
        s_y = c_y;
        c_y = call_func[i]->draw(c_x, c_y, max_height);
        drawLine(c_x, 50 + s_y);
    }

    if(max_height <(_y + height + between_functions_height_distance))
        max_height = _y + height + between_functions_height_distance;

    draw();
    if(sz == 0)
        return c_y + height + between_functions_height_distance;

    return c_y;
}

void MyFunction::drawLine(double to_x, double to_y)
{
    cr->move_to(x + width, y + (height/2));
    cr->line_to(to_x, to_y);
}

void MyFunction::drawOutline()
{
    cr->set_line_width(3);
    int block_num = calcHeight();
    cr->save();
    cr->rectangle(x,y,width,height);
    setColor();
    cr->fill_preserve();

    for(int i = 1; i < block_num; i++)
    {
        drawRowLine(i);
    }
    int start = 2;//title+argument value name
    int end = start + getArgSize();
    drawColLine(100,start,end);
    drawColLine(200,start,end);

    start = end+1;//1 : local value name
    end = start + getLocalValSize();
    drawColLine(100, start,end);
    drawColLine(200, start,end);

    UString str = getFuncName();
    drawTitle(str, 0);
    str = "parameter value";
    drawTitle(str, ch_h);
    str = "local value";
    drawTitle(str, ch_h*(2+getArgSize()));
    cr->restore();  // back to opaque black
    cr->stroke();   // outline it
}



void MyFunction::drawValues()
{
    int height_constant = 2; //title + argument title
    for(int i = 0; i <  getArgSize(); i++)
    {
        MyValue* v = getArg(i);

        UString str = v->getType();
        drawValueStr(str,0,(height_constant+i)*ch_h,100);

        str = v->getName();
        drawValueStr(str,100,(height_constant+i)*ch_h,100);

        str = v->getValue();
        drawValueStr(str,200,(height_constant+i)*ch_h,100);
    }

    height_constant += getArgSize() + 1; // 1: local value title
    for(int i = 0; i <   getLocalValSize(); i++)
    {
        MyValue* v = getLocalVal(i);

        UString str = v->getType();
        drawValueStr(str,0,(height_constant+i)*ch_h,100);

        str = v->getName();
        drawValueStr(str,100,(height_constant+i)*ch_h,100);

        str = v->getValue();
        drawValueStr(str,200,(height_constant+i)*ch_h,100);
    }



   /* height_constant += getLocalValSize() + 1; //1 : return value title
    MyValue* v = getRetVal();

    if(v)
    {
        UString str = v->getType();
        drawValueStr(str,0,(height_constant)*ch_h,100);

        str = v->getName();
        drawValueStr(str,100,(height_constant)*ch_h,100);

        str = v->getValue();
        drawValueStr(str,200,(height_constant)*ch_h,100);
    }*/

}


void MyFunction::drawRowLine(int s_b_num, double l_t)
{
    setColor(0,0,0);
    cr->set_line_width(l_t);
    cr->move_to(x, y+(s_b_num * ch_h));
    cr->line_to(x+width, y+(s_b_num * ch_h));
    cr->stroke();
}


void MyFunction::drawColLine(double st_x, int s_b_num, int b_num, double l_t)
{
    setColor(0,0,0);
    cr->set_line_width(l_t);
    cr->move_to(x+st_x, y+(s_b_num * ch_h));
    cr->line_to(x+st_x, y+(b_num * ch_h));
    cr->stroke();
}

void MyFunction::drawTitle(UString& str, int d_y)
{
    drawText(str, 0, d_y, width, Pango::WEIGHT_BOLD, 12000);
}


void MyFunction::drawValueStr(UString& str, int d_x, int d_y, int d_w)
{
    drawText(str, d_x, d_y, d_w, Pango::WEIGHT_NORMAL, 10000);
}

void MyFunction::drawText(UString& str, int d_x, int d_y, int d_w, Pango::Weight w, int font_size)
{
    setColor(0,0,0);

    Pango::FontDescription font;

     font.set_family("Monospace");
     font.set_weight(w);
     font.set_absolute_size(font_size);


     // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
     Glib::RefPtr<Pango::Layout> layout = widget->create_pango_layout(str);

     layout->set_font_description(font);

     int text_width;
     int text_height;

     //get the text dimensions (it updates the variables -- by reference)
     layout->get_pixel_size(text_width, text_height);

     // Position the text in the middle
     cr->move_to(x + d_x + (d_w-text_width)/2,y+d_y+(ch_h-text_height)/2);

     layout->show_in_cairo_context(cr);
}



UString MyFunctionManager::ID_SEPERATOR  = "-";

MyFunctionManager::MyFunctionManager()
{
    /*func_factory.clear();
    func_call_num.clear();
    dbg_persp = NULL;
    prev_func_name = "";
    id = "";
    deal_with_func = NULL;
    func_name_check = false;
    is_new_frame = false;
    addable = false;
    editable = false;
    win_x = -1;
    win_y = -1;
    max_depth = 0;
    depth = 0;*/
    clear();
    dbg_persp = NULL;
     is_continue = false;
    is_drawing = true;
    target_file_path ="";
    target_file_full_path="";
    target_line_no=0;
    target_call_no = 0;
    first_file_full_path = "";
    first_file_path = "";
    first_line_no = 0;
}

void MyFunctionManager::clear()
{

    if(func_factory.size())
    {
        for(Str_Func_Itr itr  = func_factory.begin(); itr != func_factory.end(); itr++)
        {
            delete itr->second;
        }
    }
    func_factory.clear();

    if( func_call_num.size())
    {
        for(Str_Int_Itr itr  = func_call_num.begin(); itr != func_call_num.end(); itr++)
        {
            delete itr->second;
        }
    }
    func_call_num.clear();
    prev_func_name = "";
    id = "";
    deal_with_func = NULL;
    func_name_check = false;
    is_new_frame = false;
    addable = false;
    editable = false;
    win_x = -1;
    win_y = -1;
    max_depth = 0;
    depth = 0;
    oldframenum=-1;
    is_to_bp = false;


}

bool MyFunctionManager::isDrawing()
{
    return is_drawing;
}
bool MyFunctionManager::isSameFunction()
{
    return !is_new_frame;
}
bool MyFunctionManager::isBreakPoint(UString& f_full_path, UString& f_path, int l_no)
{
    bool result = false;
    bool _false = false;
    if(f_full_path != "")
    {
        return dbg_persp->is_breakpoint_set_at_line(f_full_path, l_no, result);
    }

    if(f_path != "")
    {
        return dbg_persp->is_breakpoint_set_at_line(f_path, l_no, result);
    }
    return result;
}

bool MyFunctionManager::setBreakPoint(UString& f_full_path, UString& f_path, int l_no, bool is_set)
{
    bool b_set = isBreakPoint(f_full_path, f_path, l_no) ;
    if(b_set == is_set)
    {
        return b_set;
    }
    else
    {
        if(f_full_path != "")
            dbg_persp->toggle_breakpoint (f_full_path, l_no);
        else
            dbg_persp->toggle_breakpoint (f_path, l_no);
    }
    return b_set;
}
bool MyFunctionManager::existFunction()
{
    existFunction(id);
}

bool MyFunctionManager::existFunction(UString str)
{
    Str_Func_Itr itr = func_factory.find(str);
    return itr != func_factory.end();
}

bool MyFunctionManager::isParentFunction(UString f_n)
{

    int num = id.rfind(f_n);
    if(id != "" && f_n != "" &&!id.compare((num == std::string::npos? 0 : num),f_n.size(), f_n))
    {
        return true;
    }
    return false;
}
bool MyFunctionManager::isTarget(UString& f_full_path, UString& f_path, int l_no)
{
    if(f_full_path == target_file_full_path
           ||f_path == target_file_path)
    {
        if(l_no == target_line_no)
        {
            target_call_no--;
            if(target_call_no == 0)
            {
                return true;
            }
        }
    }
    return false;
}



void MyFunctionManager::setFuncName(UString f_n, bool is_n_f,
                                    UString f_full_path, UString f_path, int l_no)
{
    if(is_continue)
    {
        if(deal_with_func != NULL
                && is_drawing
                && deal_with_func->getFileFullPath() == f_full_path
                && deal_with_func->getFilePath() == f_path
                && deal_with_func->getLineNo() == l_no)
            return;

         if(is_drawing)
         {
             if(is_to_bp)
                  do_simulation(f_full_path, f_path, l_no);
             else
                  start_simulation(f_full_path, f_path, l_no);
         }
         else
         {
             do_quickContinue(f_full_path, f_path, l_no);
             return;
         }
    }

    is_new_frame = is_n_f;
    addable = false;
    editable = false;

    if(is_new_frame)
    {
        int call_num = addFuncCall(f_n);
        struct MyCallFunc* newCall = new struct MyCallFunc();

        ////cout<<func_call_num.size();
        if(isParentFunction(f_n))
        {
            if(id != "" && f_n != "")
                   if(id.size() == f_n.size())
                        return;
            reduceID();

            newCall->call_func = deal_with_func;
            newCall->is_function_call = false;

            deal_with_func->removeCallFunction();
            deal_with_func->releaseFocus();

            Str_Func_Itr itr = func_factory.find(id);
            deal_with_func = itr->second;
            deal_with_func->setFocus();
        }
        else
        {
            id = expandID(f_n);

            MyFunction* new_func= new MyFunction(id, f_n, call_num);

            newCall->call_func = new_func;
            newCall->is_function_call = true;

            if(deal_with_func)
            {
                deal_with_func->addCallFunc(new_func);
                deal_with_func->releaseFocus();
            }

            deal_with_func = new_func;
            func_factory.insert(Str_Func_Pair(id, deal_with_func));
        }

        call_sequence.push_back(newCall);
    }

    if(first_file_full_path == "")
    {
        first_file_full_path = f_full_path;
        first_file_path = f_path;
        first_line_no = l_no;

        //setBreakPoint(first_file_full_path,first_file_path,first_line_no, false);
    }
    deal_with_func->setFileFullPath(f_full_path);
    deal_with_func->setFilePath(f_path);
    deal_with_func->setLineNo(l_no);

    prev_func_name = f_n;
    func_name_check = true;
}

void MyFunctionManager::start_simulation(UString& f_full_path, UString& f_path, int l_no)
{
    if(!isTarget(f_full_path, f_path, l_no))
    {
        dbg_persp->debugger ()->step_in ();
    }
    else
    {
        is_continue = false;
        target_call_no = 0;
        target_file_full_path = "";
        target_file_path = "";
        target_line_no = 0;
    }
}

void MyFunctionManager::do_simulation(UString& f_full_path, UString& f_path, int l_no)
{
    if(!isBreakPoint(f_full_path, f_path, l_no))
    {
        dbg_persp->debugger ()->step_in ();
    }
    else
    {
        is_continue = false;
        is_to_bp = false;
    }
}

void MyFunctionManager::do_quickContinue(UString& f_full_path, UString& f_path, int l_no)
{
    if(!isTarget(f_full_path, f_path, l_no))
    {
        //usleep(10000);
        dbg_persp->debugger()->continue_to_position (target_file_path, target_line_no);
    }
    else
    {
        setBreakPoint(f_full_path, f_path, l_no, false);
        target_call_no = 0;
        target_file_full_path = "";
        target_file_path = "";
        target_line_no = 0;
         is_continue = false;
    }
}
UString MyFunctionManager::expandID(UString f_n)
{
    UString str = id;
    int i = 1;

    if(id != "")
        str.append(ID_SEPERATOR);
    str.append(f_n);
    id = str;

    while(existFunction(str))
    {
        str = id;
        str.append(UString::from_int(i++));
    }

    id = str;
    if(max_depth < ++depth)
    {
        max_depth = depth;
    }
    return id;
}

void MyFunctionManager::reduceID()
{
    depth--;
    id.erase(id.rfind(ID_SEPERATOR));
}

int MyFunctionManager::addFuncCall(UString f_n)
{
    Str_Int_Itr itr = func_call_num.find(f_n);
    if(itr != func_call_num.end())
    {
       return ++(*(itr->second));
    }
    else
    {
        int* num = new int;
        *num = 1;
        func_call_num.insert(Str_Int_Pair(f_n, num));
        return 1;
    }
}

MyFunction* MyFunctionManager::findHitFunction(double _x, double _y)
{
    Str_Func_Itr itr  = func_factory.begin();
    if(itr->second)
        return itr->second->find(_x, _y);
}


void MyFunctionManager::setAddable()
{
    addable = true;
}

void MyFunctionManager::setEditable()
{
    editable = true;
}

void MyFunctionManager::endDealWithFunction()
{
    func_name_check = false;
}

/*void MyFunctionManager::setFileName(UString file_name)
{
    if(!func_name_check)
    {
        LOG_DD("you must check function name!");
        return;
    }

    deal_with_func->setFileName(name);
}

void MyFunctionManager::setLineNo(int l_n)
{
    if(!func_name_check)
    {
        LOG_DD("you must check function name!");
        return;
    }

    deal_with_func->setLineNo(l_n);
}

void MyFunctionManager::setLevel(int l)
{
    level = l;
}*/

void MyFunctionManager::addArg(UString name, UString value, UString type)
{
    if(!func_name_check|| !addable || !is_drawing)

    {
        LOG_DD("you must check function name!");
        return;
    }

    deal_with_func->addArg(name, value, type);
}

void MyFunctionManager::addLocalVal(UString name, UString value, UString type)
{
    if(!func_name_check || !addable || !is_drawing)

    {
        LOG_DD("you must check function name!");
        return;
    }

    deal_with_func->addLocalVal(name, value, type);
}

void MyFunctionManager::editArg(UString name, UString value, UString type)
{
    if(!func_name_check || !editable || !is_drawing)
    {
        LOG_DD("you must check function name!");
        return;
    }

    deal_with_func->editArg(name,value, type);
}

void MyFunctionManager::editLocalVal(UString name, UString value, UString type)
{
    if(!func_name_check || !editable || !is_drawing)
       {
        LOG_DD("you must check function name!");
        return;
    }

    deal_with_func->editLocalVal(name,value, type);
}


void MyFunctionManager::onClicked(double _x, double _y)
{
    if(deal_with_func != NULL)
        deal_with_func->releaseFocus();
    deal_with_func = findHitFunction(_x, _y);
    if(deal_with_func != NULL)
    {
        deal_with_func->setFocus();
        target_file_full_path = deal_with_func->getFileFullPath();
        target_file_path = deal_with_func->getFilePath();
        target_line_no = deal_with_func->getLineNo();
        target_call_no = deal_with_func->getCallNum();

        setBreakPoint(target_file_full_path,target_file_path,target_line_no,true);
        restartQuickly();
    }
}
int MyFunctionManager::removeFunction(UString id)
{
    return func_factory.erase(id);
}

 UString MyFunctionManager::getFuncName()
 {
     return prev_func_name;
 }

 sigc::signal<void>& MyFunctionManager::getRestartSignal()
 {
     return m_restart_signal;
 }

 void MyFunctionManager::setDBGPersp(IPerspective& dbg)
 {
     dbg_persp = &dbg;
 }

 void MyFunctionManager::restartWithDrawing()
 {
     bool is_bp_exist = setBreakPoint(first_file_full_path,first_file_path,first_line_no,true);

     bool _false = false;

    clear();//redrawing
    is_drawing = true;
    dbg_persp->run();
    //m_restart_signal.emit();

    setFuncName("main", true, first_file_full_path, first_file_path, first_line_no);
    setBreakPoint(first_file_full_path,first_file_path,first_line_no,false);
 }

 void MyFunctionManager::restartWithDrawingToTarget()
 {
     is_continue = true;
     if(deal_with_func != NULL)
     {
         target_file_full_path = deal_with_func->getFileFullPath();
         target_file_path = deal_with_func->getFilePath();
         target_line_no = deal_with_func->getLineNo();
         target_call_no = deal_with_func->getCallNum();
         restartWithDrawing();
     }
 }

 void MyFunctionManager::restartQuickly()
 {
     is_continue = true;
    is_drawing = false;
    bool _false = false;

    // bool is_bp_exist = setBreakPoint(first_file_full_path,first_file_path,first_line_no,true);
    dbg_persp->run();
   // m_restart_signal.emit();

  //  setBreakPoint(first_file_full_path,first_file_path,first_line_no,is_bp_exist);
 }

 void MyFunctionManager::goToBp()
 {
     is_continue = true;
     is_to_bp = true;
     is_drawing = true;
     dbg_persp->debugger ()->step_in ();
 }


void MyFunctionManager::draw(int w, int h)
{
    Str_Func_Itr itr  = func_factory.begin();
    double _y = 0;
    if(itr->second)
        itr->second->draw(10., 10., _y);

    cout<<_y<<endl;

    if(win_x == -1)
    {
    }

    if(win_y < _y)
        win_y = _y + 10;

    double _x = max_depth * 400 - 70;
    if(win_x < _x)
    {
        win_x = _x;
    }

    /*for(Str_Func_Itr itr  = func_factory.begin(); itr != func_factory.end(); itr++, i++)
    {
        THROW_IF_FAIL(itr->second);
        UString id_s = itr->second->getID();
        int f_depth = id_s.split(ID_SEPERATOR).size();
        itr->second->releaseFocus();
        itr->second->draw(10. + i*between_functions_width_distance, 10.);
    }*/
}


 struct MyCallFunc* MyFunctionManager::getCallFuncAt(int i)
 {
     return call_sequence[i];
 }

/*
 int MyFunctionManager::getLevel()
 {
     return level;
 }

 UString MyFunctionManager::getFileName()
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return "";
     }

     return deal_with_func->getFileName();
 }

 int MyFunctionManager::getCurLineNo()
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return 0;
     }

     return deal_with_func->getCurLineNo();
 }
*/
 int MyFunctionManager::getFuncCallNum(UString f_n)
 {
     Str_Int_Itr itr = func_call_num.find(f_n);
     if(itr != func_call_num.end())
     {
        return *(itr->second);
     }
     else
     {
        return 0;
     }
 }

 int MyFunctionManager::getFuncCallNum()
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return 0;
     }

     return getFuncCallNum(prev_func_name);
 }

 int MyFunctionManager::getArgSize()
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return 0;
     }

     return deal_with_func->getArgSize();
 }

 int MyFunctionManager::getLocalValSize()
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return 0;
     }

     return deal_with_func->getLocalValSize();
 }

 double MyFunctionManager::getWinX()
 {
     return win_x;
 }

 double MyFunctionManager::getWinY()
 {
     return win_y;
 }

 MyValue* MyFunctionManager::getArg(int i)
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return NULL;
     }

     return deal_with_func->getArg(i);
 }

 MyValue* MyFunctionManager::getArg(UString key)
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return NULL;
     }

     return deal_with_func->getArg(key);
 }

 MyValue* MyFunctionManager::getVal(int i)
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return NULL;
     }

     return deal_with_func->getLocalVal(i);
 }

 MyValue* MyFunctionManager::getVal(UString key)
 {
     if(!func_name_check)
     {
         LOG_DD("you must check function name!");
         return NULL;
     }

     return deal_with_func->getLocalVal(key);
 }



Progress_MyFunction::Progress_MyFunction() : Figure()
{
    funcName = "";
    progress_width = 100;
    progress_height = 7*ch_h;
    setRGBA(0.7,0.9,0.9);
}


Progress_MyFunction::Progress_MyFunction(UString _f_n, int as) : Figure(), funcName(_f_n)
{


    progress_width = 100;
    progress_height = 7*ch_h;

    setRGBA(0.7,0.9,0.9);
}

Progress_MyFunction::~Progress_MyFunction()
{

}

UString Progress_MyFunction::getFuncName()
{
    return funcName;
}


void Progress_MyFunction::draw()
{
    if(!drawable)
        LOG_DD("can't draw function");

    if(progress_width==0||progress_height==0)
        LOG_DD ("no drawing");

    if((rgba.r ==0 && rgba.g==0 && rgba.b==0)||rgba.a==0)
        LOG_DD ("white image");

    progress_drawOutline();

    //drawValues();
}


double Progress_MyFunction::draw(double _x, double _y, double width)
{
    //progress_calcHeight();
    double c_x = _x;// +between_functions_width_distance;
    double c_y = _y +15;
    double s_y;

    setCoordinate(_x, _y);// -------------> It neeed progressbar's setcordinate..
    setWidth (width);
    draw();
    if(_x == 10)
        return c_y;

    return c_y ;//+ progress_height; //+ between_functions_height_distance;
}

void Progress_MyFunction::setWidth(double w)
{
    progress_width = w;
}

void Progress_MyFunction::progress_drawOutline()
{
    cr->set_line_width(6);
    //int block_num = progress_calcHeight();
    cr->save();
    //cr->rectangle(x,y,width,height);
    setColor();
    cr->fill_preserve();
    cr->restore();  // back to opaque black
    cr->stroke();   // outline it

  //  for(int i = 1; i < block_num; i++)
  //  {
  //      drawRowLine(i);
  //  }

    progress_drawRowLine(0);
    UString str = getFuncName();
    progress_drawTitle(str, 0, -13);


            // str = "parameter value";
    //drawTitle(str, ch_h);
    //str = "local value";
    //drawTitle(str, ch_h*(2+getArgSize()));
}

void Progress_MyFunction::progress_drawRowLine(int s_b_num, double l_t)
{
    setColor(0.2,0.4,0.3);
    cr->set_line_width(l_t);
    cr->move_to(x, y);
    cr->line_to(x+progress_width, y);//+(s_b_num * ch_h)
    cr->stroke();
}


void Progress_MyFunction::progress_drawTitle(UString& str,int d_x, int d_y)
{
    drawText(str, d_x, d_y, progress_width, Pango::WEIGHT_BOLD, 15000);
}


void Progress_MyFunction::drawText(UString& str, int d_x, int d_y, int d_w, Pango::Weight w, int font_size)
{
    setColor(0,0,0);

    Pango::FontDescription font;

     font.set_family("Monospace");
     font.set_weight(w);
     font.set_absolute_size(font_size);


     // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
     Glib::RefPtr<Pango::Layout> layout = widget->create_pango_layout(str);

     layout->set_font_description(font);

     int text_width;
     int text_height;

     //get the text dimensions (it updates the variables -- by reference)
     layout->get_pixel_size(text_width, text_height);

     // Position the text in the middle
     cr->move_to(x + d_x + (d_w-text_width)/2,y+d_y+(ch_h-text_height)/2);

     layout->show_in_cairo_context(cr);
}

Prgoress_MyFunctionManager::Prgoress_MyFunctionManager()
{

    win_x = -1;
    win_y = -1;
    oldframenum=-1;
    progressbar_chage=0;


}

 double Prgoress_MyFunctionManager::getWinX()
 {
     return win_x;
 }

 double Prgoress_MyFunctionManager::getWinY()
 {
     return win_y;
 }

 void Prgoress_MyFunctionManager::clear()
 {
     progressbar.clear();
     progressbar_num.clear();
     win_x = -1;
     win_y = -1;
     oldframenum=-1;
     progressbar_chage=0;
 }



void Prgoress_MyFunctionManager::progressbar_setFuncName(UString f_n, int framenum)
{

    if(oldframenum==-2)
        return;
    else if(oldframenum==-1)
    {
        Progress_MyFunction* newCall = new Progress_MyFunction(f_n, 1);
        progressbar.push_back(newCall);
        oldframenum=framenum;
        int *temp=new int(1);
        progressbar_num.push_back(temp);
        progressbar_chage=1;
    }
    else if(framenum==oldframenum)
        return;
    else if(framenum>oldframenum)
    {
        int m;

        if(progressbar_chage==0)
        {
            Progress_MyFunction* newCall = new Progress_MyFunction(f_n, 1);
            progressbar.push_back(newCall);
            m=++(*(progressbar_num.back()));
            progressbar_chage=1;
        }
        else if(progressbar_chage==1)
        {
            Progress_MyFunction* newCall = new Progress_MyFunction(f_n, 1);
            progressbar.push_back(newCall);
            //m=++(*(progressbar_num.back()));
            int *temp=new int(1);
            progressbar_num.push_back(temp);
            progressbar_chage=1;

        }
        else if(progressbar_chage==-1)
        {
            Progress_MyFunction* newCall = new Progress_MyFunction(f_n, 1);
            progressbar.push_back(newCall);
            (*(progressbar_num.back()))++;
            progressbar_chage=0;

        }
        oldframenum=framenum;
    }
    else if(framenum<oldframenum)
    {
         if(progressbar_chage==1)
        {
            //Progress_MyFunction* newCall = new Progress_MyFunction(f_n, 1);
            //progressbar.push_back(newCall);
            //(*(progressbar_num.back()))++;
            progressbar_chage=0;

        }
       else if(progressbar_chage==0)
        {
            int *delnum=(((progressbar_num.back())));
            for(int i=0;i<*delnum;i++)
                progressbar.erase(progressbar.end()-1);
            progressbar_num.erase(progressbar_num.end()-1);
            progressbar_chage=-1;

        }
        else if(progressbar_chage==-1)
        {
             int *delnum=((progressbar_num.back()));
            for(int i=0;i<*delnum;i++)
                progressbar.erase(progressbar.end()-1);
            progressbar_num.erase(progressbar_num.end()-1);
            progressbar_chage=-1;

        }
        if(framenum==1&&oldframenum==1)
               oldframenum=-2;
        else
            oldframenum=framenum;

    }

    //std::cout<<f_n<<endl;

}

void Prgoress_MyFunctionManager::draw(int w, int h)
{
  //  Str_Func_Itr itr  = func_factory.begin();
    double _y;

    int progress_x=10;
    int progress_y=10;
    int progress_width=600;
    int child_num;
    double previndent=0;
    int i,j,k;
    //j, k with you must draw

    int size =progressbar.size();
    int numsize=progressbar_num.size();
    for(i = 0; i < size;i++)
    {
        for(j=0; j<numsize;j++)

        {
             child_num= *(progressbar_num[j]);
             for(k=0; k<child_num;k++,i++)
                  (*progressbar[i]).draw(progress_x+previndent+k*progress_width/(child_num),progress_y+i*25, (progress_width/(child_num)-5));
             previndent=previndent+(k-1)*progress_width/(child_num);
             progress_width=progress_width/(child_num);
        }
    }
//progress_width/(child_num*(2*(j+1)))


  //  if(itr->second)
  //      _y = itr->second->progress_draw(10., 10.);



    if(win_x == -1)
    {
        win_x = w;
        win_y = h;
    }

    if(win_y < _y)
        win_y = _y;
    int _x=200;
    if(win_x < _x)
    {
        win_x = _x;
    }
    win_y=50*progressbar.size();
        

}
}//end namespace nemiver
