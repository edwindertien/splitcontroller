#!/usr/bin/python

import sys
#import csv
import pandas as pd
from PyQt5.QtWidgets import QMessageBox, QAction, QWidget, QApplication, QDialog, QGridLayout, QLabel, QLineEdit, QFileDialog, QScrollArea, QPushButton, QComboBox
from PyQt5.Qt import QHBoxLayout, QWindow, QMainWindow, QVBoxLayout

ps4btns = ["joyRXax2", "joyRYax1", "joyRZb11", "joyLZb12", "joyLYax4", "joyLXax3", "sqbutton", "circbutton", "psbutton", "l2button", "r2button", "tributton", "xbutton", "lhat", "rhat", "uhat", "dhat", "r1button6", "l1button5", "optbutton", "accx1", "accy1", "accx2", "accy2", "c_button1", "z_button1", "c_button2", "z_button2", "shrebutton", "PWM1", "PWM2", "PWM3", "PWM4", "PWM5"]
menus = []

class MainWindow(QMainWindow):
    path = None

    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        dataHandler = DataHandler()
        self.initUI()

    def initUI(self):
        self.vlayoutmain = QVBoxLayout()
        self.hlayoutsub1 = QHBoxLayout()
        self.hlayoutsub2 = QHBoxLayout()
        self.hlayoutsub3 = QHBoxLayout()
        self.hlayoutsub4 = QHBoxLayout()
        self.hlayoutsubBttm = QHBoxLayout()
        self.widget = QWidget()
        self.widget.setLayout(self.vlayoutmain)
        # setup of top layout
        self.label1 = QLabel('Please choose a file or start from scratch')
        self.hlayoutsub1.addWidget(self.label1)
        # next row
        self.btnChooseFile = QPushButton("Open File")
        self.btnChooseFile.clicked.connect(self.open)  # connect clicked to self.open()
        self.hlayoutsub2.addWidget(self.btnChooseFile)
        # next row
        self.label2 = QLabel('Choose a menu to edit or make a new one')
        self.hlayoutsub3.addWidget(self.label2)
        # next row
        self.chooseMenuCombo = QComboBox()
        for thing in range(len(menus)):
            self.chooseMenuCombo.addItem(thing)
        self.hlayoutsub4.addWidget(self.chooseMenuCombo)
        self.chosenButton = QPushButton("Edit this menu")
        self.chosenButton.clicked.connect(self.choice)
        self.hlayoutsub4.addWidget(self.chosenButton)
        self.txtMenu = QLineEdit()
        self.hlayoutsub4.addWidget(self.txtMenu)
        self.makeButton = QPushButton("Make this menu")
        self.makeButton.clicked.connect(self.make)
        self.hlayoutsub4.addWidget(self.makeButton)

        # make grid in scrollarea
        self.hlayoutsub5 = QHBoxLayout()
        self.scrollArea = QScrollArea()
        self.scrollArea.setWidgetResizable(True)
        self.scrollAreaWidgetContents = QWidget()
        self.gridlayout = QGridLayout(self.scrollAreaWidgetContents)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        self.hlayoutsub5.addWidget(self.scrollArea)

        # read CSV data

        
        self.gridlayout.setColumnStretch(1, 4)
        self.gridlayout.setColumnStretch(2, 4)
        self.gridlayout.addWidget(QPushButton('1'), 0, 0)
        self.gridlayout.addWidget(QPushButton('2'), 0, 1)
        self.gridlayout.addWidget(QPushButton('3'), 0, 2)
        self.gridlayout.addWidget(QPushButton('4'), 1, 0)
        self.gridlayout.addWidget(QPushButton('5'), 1, 1)
        self.gridlayout.addWidget(QPushButton('6'), 1, 2)
        self.gridlayout.addWidget(QPushButton('7'), 2, 0)
        self.gridlayout.addWidget(QPushButton('8'), 2, 1)
        self.gridlayout.addWidget(QPushButton('9'), 2, 2)

        # next

        self.hlayoutsub1.addStretch(2)

        self.hlayoutsub2.addStretch(1)

        # for save file later
        self.saveButton = QPushButton("Save edited menu")
        self.saveButton.clicked.connect(self.file_save)
        self.hlayoutsubBttm.addWidget(self.saveButton)

        

        # finally


        self.vlayoutmain.addLayout(self.hlayoutsub1)
        self.vlayoutmain.addLayout(self.hlayoutsub2)
        self.vlayoutmain.addLayout(self.hlayoutsub3)
        self.vlayoutmain.addLayout(self.hlayoutsub4)
        self.vlayoutmain.addLayout(self.hlayoutsub5)
        self.vlayoutmain.addLayout(self.hlayoutsubBttm)


        self.vlayoutmain.addStretch(1)

        self.setCentralWidget(self.widget)

        self.setGeometry(500, 500, 500, 500)
        self.setWindowTitle('Edit splitcontroller configuration')
        self.show()

    def open(self):
        nwpath = QFileDialog.getOpenFileName(self, 'Open a file', '',
                                           'All Files (*.*)')
        if self.path != ('', ''):
            print("File path : " + nwpath[0])
            self.path = nwpath[0]
            csvdata = dataHandler.readCSV(self.path)
            if csvdata is pd.DataFrame:
                pass
            elif csvdata is str:
                alrt = QMessageBox()
                alrt.setWIndowTitle("An error occurred")
                alrt.setText(csvdata)

            

    def choice(self):
        list = []
        return list

    def make(self):
        pass

    def file_save(self):
        name = QtGui.QFileDialog.getSaveFileName(self, 'Save File')
        file = open(name,'w')
        text = self.textEdit.toPlainText()
        file.write(text)
        file.close()

class DataHandler():
    def readCSV(path):
        try:
            # change 'Name' with first column name, so first word in csv
            df = pd.read_csv(path, index_col='menu')
            print(df)
            return df
        except:
            msg = "Failed to read CSV"
            return msg
        
    def writeCSV(df):
        pd.df.to_csv('mapping_modified.csv')

if __name__ == '__main__':

    app = QApplication(sys.argv)
    ex = MainWindow()
    sys.exit(app.exec_())


