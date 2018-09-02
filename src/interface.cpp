#include "interface.h"


Interface::Interface(QWidget *parent):QWidget(parent),gui(new Ui::InterfaceUI){
    gui->setupUi(this);
}
