//    deltanote.cpp: Source file for Deltanote class
//    Copyright (C) 2014  Nathan Robert Yee

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <QFile>
#include <QTextStream>
#include <QDir>

#include "deltanote.h"
#include "ui_deltanote.h"

Deltanote::Deltanote(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Deltanote)
{
    ui->setupUi(this);
}

Deltanote::~Deltanote()
{
    delete ui;
}

void Deltanote::on_textEdit_textChanged()
{
    QString homeDir = QDir::homePath();
    QString filePath = homeDir.append("/.deltanote");
    if (!QDir(filePath).exists()) {
        QDir(filePath).mkpath(".");
    }

    filePath = homeDir.append("/1");
    if (!filePath.isEmpty()) {
       QFile file(filePath);
       if (!file.open(QIODevice::WriteOnly)) {
           // TODO: Improve error handling
           return;
       } else {
           QTextStream textStream(&file);
           textStream << ui->textEdit->toPlainText();
           textStream.flush();
           file.close();
       }
    }
}
