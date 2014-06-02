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

// NOTE: Consider changing to enum class for C++11
enum TreeViewfsModelColumns {TVFSMC_NAME, TVFSMC_SIZE, TVFSMC_TYPE,
                             TVFSMC_DATE_MODIFIED};
static QString currentFilePath;

Deltanote::Deltanote(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Deltanote)
{
    ui->setupUi(this);
    ui->splitter_2->setStretchFactor(1,1);

    fsModel = new QFileSystemModel(this);
    fsModel->setReadOnly(true);

    QString homeDir = QDir::homePath();
    currentFilePath = homeDir.append("/.deltanote");
    if (!QDir(currentFilePath).exists()) {
        QDir(currentFilePath).mkpath(".");
    }
    fsModel->setRootPath(currentFilePath);

    ui->treeView->setModel(fsModel);
    ui->treeView->setRootIndex(fsModel->index(currentFilePath));
    ui->treeView->hideColumn(TVFSMC_SIZE);
    ui->treeView->hideColumn(TVFSMC_TYPE);
}

Deltanote::~Deltanote()
{
    delete ui;
}

void Deltanote::on_textEdit_textChanged()
{
    QString homeDir = QDir::homePath();
    currentFilePath = homeDir.append("/.deltanote");
    if (!QDir(currentFilePath).exists()) {
        QDir(currentFilePath).mkpath(".");
    }

    currentFilePath = homeDir.append("/Note");
    if (!currentFilePath.isEmpty()) {
       QFile file(currentFilePath);
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
