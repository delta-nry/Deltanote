/*!
\file    deltanote.h
\author  Nathan Robert Yee

\section LICENSE

deltanote.h: Header file for Deltanote class
Copyright (C) 2014  Nathan Robert Yee

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DELTANOTE_H
#define DELTANOTE_H

#include <QMainWindow>
#include <QFileSystemModel>

#include "note.h"

namespace Ui {
class Deltanote;
}

class Deltanote : public QMainWindow
{
    Q_OBJECT

public:
    explicit Deltanote(QWidget *parent = 0);
    ~Deltanote();

private slots:
    void on_textEdit_textChanged();
    void on_lineEdit_editingFinished();
    void on_addNoteButton_clicked();
    void on_deleteButton_clicked();
    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::Deltanote *ui;
    QFileSystemModel *fsModel;

    bool openFromFile(QString filepath);
    bool recordLastNote();
    bool switchNote(QDir path);
    bool removeNote(QString path);
    QString getBaseNotePath();
    QString getLastNoteSettingsPath();
};

#endif // DELTANOTE_H
