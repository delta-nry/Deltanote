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
#include "note.h"

// NOTE: Consider changing to enum class for C++11
enum TreeViewfsModelColumns {TVFSMC_NAME, TVFSMC_SIZE, TVFSMC_TYPE,
                             TVFSMC_DATE_MODIFIED};
// Only one note (and therefore one filepath) can currently be active at a time
static Note activeNote;

Deltanote::Deltanote(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Deltanote)
{
    ui->setupUi(this);
    // Set initial splitter ratio
    ui->splitter_2->setStretchFactor(1,1);

    fsModel = new QFileSystemModel(this);
    fsModel->setReadOnly(true);
    fsModel->setRootPath(getBaseNotePath());

    ui->treeView->setModel(fsModel);
    ui->treeView->setRootIndex(fsModel->index(getBaseNotePath()));
    ui->treeView->hideColumn(TVFSMC_SIZE);
    ui->treeView->hideColumn(TVFSMC_TYPE);
    // XXX: Do not hide date modified column once QFileSystemModel file
    // modification bug is fixed
    ui->treeView->hideColumn(TVFSMC_DATE_MODIFIED);

    // Attempt to load last recently used note, else create a new note
    QString openSettingsPath = getLastNoteSettingsPath();
    if (!openNote(openSettingsPath)) {
        // Auto-load failed; attempt to create default note "Note" in
        // BaseRootPath if it does not exist, then open it
        activeNote = Note(QDir(getBaseNotePath() + "/Note"));
        ui->lineEdit->setText(activeNote.name());
        ui->textEdit->setText(activeNote.read());
    }
}

Deltanote::~Deltanote()
{
    if (!saveNote()) {
        QApplication::quit();
    }
    delete ui;
}

void Deltanote::on_textEdit_textChanged()
{
    activeNote.write(ui->textEdit->toPlainText());
}

void Deltanote::on_lineEdit_editingFinished()
{
    QString originalName = activeNote.name();
    // Reload old note name into lineEdit if new note name is invalid
    if (!(activeNote.rename(ui->lineEdit->displayText()))) {
        ui->lineEdit->setText(originalName);
    }
}

// Return false if load fails, else return true
bool Deltanote::openNote(QString filepath)
{
    if (!filepath.isEmpty()) {
        QFile file(filepath);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream ts(&file);
            // Remember to remove trailing newline from textStream.readAll()
            // QString::simplified() does this for you
            QString loadpath = ts.readAll().simplified();
            ts.flush();
            if (QDir(getBaseNotePath()).exists(loadpath)) {
                activeNote = Note(QDir(loadpath));
                ui->lineEdit->setText(activeNote.name());
                ui->textEdit->setText(activeNote.read());
                file.close();
                return true;
            }
            file.close();
        }
    }
    return false;
}

// Returns false if save operation fails
// Write last recently active note's filepath into
// [HOME]/.config/deltanote/lastnote to be auto-loaded on next init
bool Deltanote::saveNote()
{
    QString saveSettingsPath = getLastNoteSettingsPath();
    if (!saveSettingsPath.isEmpty()) {
        QFile file(saveSettingsPath);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream ts(&file);
            ts << activeNote.filepath();
            ts.flush();
            file.close();
            return true;
        }
    }
    return false;
}

// BaseNotePath is [HOME]/.deltanote
QString Deltanote::getBaseNotePath()
{
    QString currentNotePath = QDir::homePath() + "/.deltanote";
    if (!QDir(currentNotePath).exists()) {
        QDir(currentNotePath).mkpath(".");
    }
    return currentNotePath;
}

// LastNotePath is [HOME]/.config/deltanote/lastnote
QString Deltanote::getLastNoteSettingsPath()
{
    return (QDir::homePath() + "/.config/deltanote/lastnote");
}
