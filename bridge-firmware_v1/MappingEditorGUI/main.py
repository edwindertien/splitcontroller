#!/usr/bin/python

import sys
from PyQt5.QtWidgets import QWidget, QApplication, QDialog, QGridLayout, QLabel, QLineEdit, QFileDialog, QPushButton, QComboBox
from PyQt5.Qt import QHBoxLayout, QWindow, QMainWindow, QVBoxLayout

ps4btns = ["joyRXax2", "joyRYax1", "joyRZb11", "joyLZb12", "joyLYax4", "joyLXax3", "sqbutton", "circbutton", "psbutton", "l2button", "r2button", "tributton", "xbutton", "lhat", "rhat", "uhat", "dhat", "r1button6", "l1button5", "optbutton", "accx1", "accy1", "accx2", "accy2", "c_button1", "z_button1", "c_button2", "z_button2", "shrebutton", "PWM1", "PWM2", "PWM3", "PWM4", "PWM5"]
menus = []

class MainWindow(QMainWindow):


    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        self.initUI()

    def initUI(self):
        vlayoutmain = QVBoxLayout()
        hlayoutsub1 = QHBoxLayout()
        hlayoutsub2 = QHBoxLayout()
        hlayoutsub3 = QHBoxLayout()
        hlayoutsub4 = QHBoxLayout()
        widget = QWidget()
        widget.setLayout(vlayoutmain)
        # setup of top layout
        label1 = QLabel('Please choose a file or start from scratch')
        hlayoutsub1.addWidget(label1)
        # next row
        btnChooseFile = QPushButton("Open File")
        btnChooseFile.clicked.connect(self.open)  # connect clicked to self.open()
        hlayoutsub2.addWidget(btnChooseFile)
        # next row
        label2 = QLabel('Choose a menu to edit or make a new one')
        hlayoutsub3.addWidget(label2)
        # next row
        chooseMenuCombo = QComboBox()
        for thing in range(len(menus)):
            chooseMenuCombo.addItem(thing)
        hlayoutsub4.addWidget(chooseMenuCombo)
        chosenButton = QPushButton("Edit this menu")
        chosenButton.clicked.connect(self.choice)

        # make grid
        gridlayout = QGridLayout()
        gridlayout.setColumnStretch(1, 4)
        gridlayout.setColumnStretch(2, 4)
        gridlayout.addWidget(QPushButton('1'), 0, 0)
        gridlayout.addWidget(QPushButton('2'), 0, 1)
        gridlayout.addWidget(QPushButton('3'), 0, 2)
        gridlayout.addWidget(QPushButton('4'), 1, 0)
        gridlayout.addWidget(QPushButton('5'), 1, 1)
        gridlayout.addWidget(QPushButton('6'), 1, 2)
        gridlayout.addWidget(QPushButton('7'), 2, 0)
        gridlayout.addWidget(QPushButton('8'), 2, 1)
        gridlayout.addWidget(QPushButton('9'), 2, 2)

        # next

        hlayoutsub1.addStretch(2)

        hlayoutsub2.addStretch(1)


        vlayoutmain.addLayout(hlayoutsub1)
        vlayoutmain.addLayout(hlayoutsub2)
        vlayoutmain.addLayout(hlayoutsub3)
        vlayoutmain.addLayout(gridlayout)


        vlayoutmain.addStretch(1)

        self.setCentralWidget(widget)

        self.setGeometry(500, 500, 500, 500)
        self.setWindowTitle('Edit splitcontroller configuration')
        self.show()

    def open(self):
        path = QFileDialog.getOpenFileName(self, 'Open a file', '',
                                           'All Files (*.*)')
        if path != ('', ''):
            print("File path : " + path[0])

    def choice(self):
        list = []
        return list

if __name__ == '__main__':

    app = QApplication(sys.argv)
    ex = MainWindow()
#     ex.show()
    sys.exit(app.exec_())


