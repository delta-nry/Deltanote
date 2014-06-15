/*!
\file    deltanote.cpp
\author  Nathan Robert Yee

\section LICENSE

deltanote.cpp: Implementation file for Deltanote class
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

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QShortcut>

#include "deltanote.h"
#include "ui_deltanote.h"
#include "note.h"

/*!
 * \brief Constructor of Deltanote application.
 *
 * Initializes UI elements and attempts to open the last-used-note used by
 * Deltanote. If the open operation succeeds, then the opened note becomes the
 * active note. If the open operation fails then a default note "New Note" is
 * opened or created if it does not exist and becomes the active note.
 *
 * \param parent
 */
Deltanote::Deltanote(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Deltanote)
{
    enum TreeViewfsModelColumns {TVFSMC_NAME, TVFSMC_SIZE, TVFSMC_TYPE,
                                 TVFSMC_DATE_MODIFIED};

    // Add custom-defined keyboard shortcuts
    new QShortcut(QKeySequence(tr("Ctrl+Q", "Quit")), this, SLOT(close()));
    new QShortcut(QKeySequence(tr("Ctrl+W", "Quit")), this, SLOT(close()));

    // Setup UI
    ui->setupUi(this);
    // Set initial splitter ratio
    ui->splitter_2->setStretchFactor(1,1);

    // Set up sidebar note picker
    fsModel = new QFileSystemModel(this);
    fsModel->setReadOnly(true);
    fsModel->setRootPath(getBaseNotePath());
    ui->treeView->setModel(fsModel);
    ui->treeView->setRootIndex(fsModel->index(getBaseNotePath()));
    ui->treeView->hideColumn(TVFSMC_SIZE);
    ui->treeView->hideColumn(TVFSMC_TYPE);

    // Attempt to load last recently used note, else create a new note
    QString openSettingsPath = getLastNoteSettingsPath();
    if (!openFromFile(openSettingsPath)) {
        // Auto-load failed; attempt to create default note "New Note" in
        // [HOME]/.deltanote if it does not exist, then open it
        if (!switchNote(QDir(getBaseNotePath() + "/New Note"))) {
            qFatal("Initialization note loading failed");
            QApplication::quit();
        }
    }
}

/*!
 * \brief Destructor of Deltanote application.
 *
 * Saves active note and deletes UI elements.
 */
Deltanote::~Deltanote()
{
    if (!recordLastNote()) {
        qWarning("last-recently-used note not recorded");
        QApplication::quit();
    }
    delete ui;
}

/*!
 * \brief Saves content to the active note.
 *
 * Saves content to the active note when the note content buffer is
 * changed.
 */
void Deltanote::on_textEdit_textChanged()
{
    activeNote.write(ui->textEdit->toPlainText());
}

/*!
 * \brief Attempts to rename the active note.
 *
 * Attempts to rename the active note to the note name buffer contents after
 * editing of the buffer finishes. If the rename operation succeeds, the
 * renamed note becomes the active note. If a note with the same name as the
 * name buffer contents exists in the folder of the active note, the name of
 * the active note is unchanged and the contents of the name buffer reverts to
 * the original name of the active note.
 */
void Deltanote::on_lineEdit_editingFinished()
{
    QString originalName = activeNote.name();
    // Reload old note name into lineEdit if new note name is invalid
    if (!(activeNote.rename(ui->lineEdit->displayText()))) {
        ui->lineEdit->setText(originalName);
    }
}

/*!
 * \brief Attempts to create a new note and open it.
 *
 * Attempts to create a new note and open it. If all operations are successful,
 * the opened note becomes the active note. If note creation fails, no new
 * note is created, and if note opening fails, the previous active note remains
 * the active note with its contents loaded in the UI.
 */
void Deltanote::on_addNoteButton_clicked()
{
    // Save active note and load note "New Note" or create it if it does not
    // exist
    activeNote.write(ui->textEdit->toPlainText());
    // Attempt to create "New Note". If "New Note" exists create "New Note 2"
    // up until "New Note 1024". If "New Note" and "New Note 2" to
    // "New Note 1024" exist, do nothing
    if (QDir(getBaseNotePath()).exists("New Note")) {
        for (int i = 2; i <= 1024; i++) {
            if (!QDir(getBaseNotePath()).exists("New Note "
                                                + QString::number(i))) {
                if (!switchNote(QDir(getBaseNotePath() + "/New Note "
                                     + QString::number(i)))) {
                    qWarning("New note creation failed");
                    return;
                }
                // Select new file in sidebar
                // TODO: Check if setCurrentIndex is successful
                ui->treeView->setCurrentIndex(
                            fsModel->index(activeNote.path()));
                break;
            }
        }
    } else {
        if (!switchNote(QDir(getBaseNotePath() + "/New Note"))) {
            qWarning("New note creation failed");
            return;
        }
    }
}

/*!
 * \brief Attempts to remove the current active note.
 */
void Deltanote::on_deleteButton_clicked()
{
    if (!removeNote(activeNote.path())) {
        qWarning("Note removal failed");
    }
}

/*!
 * \brief Attempts to open the selected note.
 *
 * Attempts to open the selected note and if successful makes it the active
 * note. If the note is already the active note or note opening fails, then the
 * previously active note is selected and its contents remain in the UI.
 *
 * \param &index The selected file in a QTreeView.
 */
void Deltanote::on_treeView_clicked(const QModelIndex &index)
{
    // Check which note is selected in the sidebar and load it if it is not
    // already the active note
    activeNote.write(ui->textEdit->toPlainText());
    if (!switchNote(QDir(fsModel->fileInfo(index).absoluteFilePath()))) {
        qWarning("Note opening failed");
    }
}

/*!
 * \brief Attempts to open an existing note on the filesystem from a file.
 *
 * Attempts to open an existing note on the filesystem from a file and if
 * successful makes it the active note and returns true. Returns false if the
 * open operation fails.
 *
 * \warning The file at filepath must only contain an absolute filepath.
 *
 * \param filepath The location of the file to be opened.
 *
 * \return true on success, false otherwise.
 */
bool Deltanote::openFromFile(QString filepath)
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
                // loadpath must be an absolute filepath
                activeNote = Note(QDir(loadpath));
                ui->lineEdit->setText(activeNote.name());
                ui->textEdit->setText(activeNote.read());
                // Select new file in sidebar
                // TODO: Check if setCurrentIndex is successful
                ui->treeView->setCurrentIndex(
                            fsModel->index(activeNote.path()));
                file.close();
                return true;
            }
            file.close();
        }
    }
    return false;
}

/*!
 * \brief Attempts to record the active note as last-recently-used on disk.
 *
 * Attempts to record the active note as last-recently-used on disk and if
 * successful returns true. If the recording operation fails then any changes
 * made on disk are reverted back to its state before the save operation and
 * returns false.
 *
 * \return true on success, false otherwise.
 */
// Returns false if save operation fails
// Write last recently active note's filepath into
// [HOME]/.config/deltanote/lastnote to be auto-loaded on next init
bool Deltanote::recordLastNote()
{
    QString saveSettingsPath = getLastNoteSettingsPath();
    if (!saveSettingsPath.isEmpty()) {
        QFile file(saveSettingsPath);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream ts(&file);
            ts << activeNote.path();
            ts.flush();
            file.close();
            return true;
        }
        qWarning("Deltanote::recordLastNote(): "
                 "Could not open file in lastNoteSettingsPath");
    }
    qWarning("Deltanote::recordLastNote(): "
             "Could not find lastNoteSettingsPath: %s"
             , saveSettingsPath.toStdString().c_str());
    return false;
}

/*!
 * \brief Switches the current active note to a new active note.
 *
 * Switches the current active note to a new active note and if successful
 * updates relevant UI elements and returns true.
 *
 * \warning Always returns true; to be fixed in the future. Undefined behavior
 * if note switching or UI updating fail.
 *
 * \param path The absolute path to the note which is to replace the current
 * active note.
 *
 * \return true on success, true otherwise.
 */
bool Deltanote::switchNote(QDir path)
{
    activeNote = Note(path);
    ui->lineEdit->setText(activeNote.name());
    ui->textEdit->setText(activeNote.read());
    return true;
    // TODO: Add return false on error
}

/*!
 * \brief Removes the note at a given path.
 *
 * If the remove operation is successful and files exist in the parent
 * directory of path after the operation, the active note is switched to the
 * note in the directory with the most recent date modified attribute and is
 * opened. If no notes exist in the directory after the note is removed, the
 * note "New Note" is created, becomes the active note and is opened. If the
 * remove operation fails, no changes are made to the filesystem.
 *
 * \param path The absolute path to the note to be deleted.
 *
 * \return true on success, false otherwise.
 */
// NOTE: Consider changing to: "If no notes exist on the filesystem after the
// note is removed, the note name and note contents buffers are cleared."
// instead of creating/opening "New Note"
bool Deltanote::removeNote(QString path)
{
    if (QFile(path).exists()) {
        if (QFile(path).remove()) {
            QDir dir = QDir(path);
            dir.cdUp();
            QFileInfoList fil = dir.entryInfoList(QDir::Files);
            // If fil holds one or zero notes, create and/or open "New Note"
            if (fil.size() == 0) {
                if (!switchNote(QDir(getBaseNotePath() + "/New Note"))) {
                    qWarning("Note creation/opening failed");
                    return false;
                }
                // Select new file in sidebar
                // TODO: Check if setCurrentIndex is successful
                ui->treeView->setCurrentIndex(
                            fsModel->index(activeNote.path()));
                return true;
            } else if (fil.size() == 1) {
                if (!switchNote(QDir(fil.at(0).absoluteFilePath()))) {
                    qWarning("Note switching failed");
                    return false;
                }
                return true;
            }
            // Check for the most-recently-modified file in path's parent
            // directory
            QFileInfo mostRecent = fil.at(0);
            QFileInfo checkRecent;
            for (int i = 1; i < fil.size(); i++) {
                checkRecent = fil.at(i);
                if (mostRecent.lastModified() < checkRecent.lastModified()) {
                    mostRecent = checkRecent;
                }
            }
            if (!switchNote(QDir(mostRecent.absoluteFilePath()))) {
                qWarning("Note switching failed");
                return false;
            }
            return true;
        }
    }
    return false;
}

/*!
 * \brief Get the path of Deltanote's base directory
 *
 * The base directory of Deltanote is in "[HOME]/.deltanote".
 *
 * \return A QString representing Deltanote's base directory.
 */
QString Deltanote::getBaseNotePath()
{
    QString currentNotePath = QDir::homePath() + "/.deltanote";
    if (!QDir(currentNotePath).exists()) {
        QDir(currentNotePath).mkpath(".");
    }
    return currentNotePath;
}

/*!
 * \brief Get the path of the file containing Deltanote's last-used-note
 * information.
 *
 * The file is in "[HOME]/.config/deltanote/lastnote".
 *
 * \return A QString representing the path of the last-used-note.
 */
QString Deltanote::getLastNoteSettingsPath()
{
    QString lastNoteSettingsPath =
            QDir::homePath() + "/.config/deltanote";
    if (!QDir(lastNoteSettingsPath).exists()) {
        QDir(lastNoteSettingsPath).mkpath(".");
    }
    lastNoteSettingsPath.append("/lastnote");
    return lastNoteSettingsPath;
}
