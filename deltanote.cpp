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
// Only one file path (and therefore note) can currently be active at a time
static QString currentNotePath;
// Stores the name of the currently active note
static QString currentNoteName;

Deltanote::Deltanote(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Deltanote)
{
    ui->setupUi(this);
    // Set initial splitter ratio
    ui->splitter_2->setStretchFactor(1,1);

    fsModel = new QFileSystemModel(this);
    fsModel->setReadOnly(true);

    setMainCurrentNotePath();
    fsModel->setRootPath(currentNotePath);

    ui->treeView->setModel(fsModel);
    ui->treeView->setRootIndex(fsModel->index(currentNotePath));
    ui->treeView->hideColumn(TVFSMC_SIZE);
    ui->treeView->hideColumn(TVFSMC_TYPE);
    // XXX: Do not hide date modified column once QFileSystemModel file
    // modification bug is fixed
    ui->treeView->hideColumn(TVFSMC_DATE_MODIFIED);

    // BEGIN BLOCK: Load previous note into QTextEdit and QLineEdit on init
    setMainCurrentNotePath();
    // XXX: Hardcoded value
    currentNoteName = "/Note";
    currentNotePath.append(currentNoteName);
    if (!currentNotePath.isEmpty()) {
       QFile file(currentNotePath);
       if (!file.open(QIODevice::ReadOnly)) {
           // TODO: Improve error handling
           return;
       } else {
           QTextStream textStream(&file);
           ui->textEdit->setText(textStream.readAll());
           ui->lineEdit->setText(currentNoteName);
           textStream.flush();
           file.close();
       }
    }
    // END BLOCK
}

Deltanote::~Deltanote()
{
    delete ui;
}

void Deltanote::on_textEdit_textChanged()
{
    setMainCurrentNotePath();
    if (currentNoteName.isEmpty()) {
        // TODO: Improve error handling
        return;
    } else {
        currentNotePath.append(currentNoteName);
        if (!currentNotePath.isEmpty()) {
           QFile file(currentNotePath);
           if (!file.open(QIODevice::WriteOnly)) {
               // TODO: Improve error handling
               return;
           } else {
               QTextStream textStream(&file);
               textStream << ui->textEdit->toPlainText();
               ui->lineEdit->setText(currentNoteName);
               textStream.flush();
               file.close();
           }
        }
    }
}

// Sets currentNotePath to [HOME]/.deltanote
void Deltanote::setMainCurrentNotePath() {
    QString homeDir = QDir::homePath();
    currentNotePath = homeDir.append("/.deltanote");
    if (!QDir(currentNotePath).exists()) {
        QDir(currentNotePath).mkpath(".");
    }
}
