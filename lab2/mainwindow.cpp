#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QColorDialog>
#include <QFontDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textChanged = false;

    statusLabel.setMaximumWidth(180);
    statusLabel.setText("length: "+QString::number(0)+"   lines: "+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(180);
    statusCursorLabel.setText("Ln: "+QString::number(0)+"   Col: "+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText(tr("欧阳秋纹"));
    ui->statusbar->addPermanentWidget(author);

    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionPaste->setEnabled(false);

    QPlainTextEdit::LineWrapMode mode = ui->textEdit->lineWrapMode();

    if(mode==QPlainTextEdit::NoWrap){
        ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        ui->actionLineWrap->setChecked(false);
    } else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actionLineWrap->setChecked(true);
    }

    ui->actionToolbar->setChecked(true);
    ui->actionStatusbar->setChecked(true);
    ui->actionShowLineNumbers->setChecked(false);
    // on_actionShowLineNumbers_triggered(false);

    connect(ui->actionShowLineNumbers,SIGNAL(triggered(bool)),ui->textEdit,SLOT(hideLineNumberArea(bool)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()
{
    SearchDialog dlg(this,ui->textEdit);
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg(this,ui->textEdit);
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()
{
    if(!userEditConfirmed()){
        return;
    }

    filePath = "";

    ui->textEdit->clear();
    this->setWindowTitle(tr("新建文本文件 - 编辑器"));

    textChanged = false;
}


void MainWindow::on_actionOpen_triggered()
{
    if(!userEditConfirmed()){
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text file (*.txt) ;; All(*.*)"));
    if(filename.isEmpty()){              // 用户取消
        return;
    }

    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "..", "打开文件失败");
        return;
    }

    filePath = filename;

    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setPlainText(text);    // 替换而不是追加
    file.close();

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());
    textChanged = false;
}


void MainWindow::on_actionSave_triggered()
{
    if(filePath == ""){
        QString filename = QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text file (*.txt)"));
        if(filename.isEmpty()){          // 用户取消
            return;
        }
        filePath = filename;
    }

    QFile file(filePath);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "..", "保存文件失败");
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
    file.flush();
    file.close();

    textChanged = false;                 // 关键：复位
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text file (*.txt)"));
    if(filename.isEmpty()){
        return;
    }

    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "..", "保存文件失败");
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
    file.flush();
    file.close();

    filePath = filename;
    textChanged = false;
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}


void MainWindow::on_textEdit_textChanged()
{
    if(!textChanged){
        this->setWindowTitle("*"+this->windowTitle());
        textChanged=true;
    }

    statusLabel.setText("length: "+QString::number(ui->textEdit->toPlainText().length())+"   lines: "+
                        QString::number(ui->textEdit->document()->lineCount()));


}

bool MainWindow::userEditConfirmed()
{
    if(textChanged){

        QString path = (filePath !="") ? filePath:"无标题.txt";

        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n") + "\"" + path + "\" ?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        int r =msg.exec();
        switch(r){
        case QMessageBox::Yes:
            on_actionSave_triggered();

            if(textChanged){          // 保存失败或用户取消了保存对话框
                return false;
            }
            break;

        case QMessageBox::No:
            textChanged = false;
            break;

        case QMessageBox::Cancel:
            return false;

        default:
                return false;
        }

        return true;
    }
     return true;
}


void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}


void MainWindow::on_textEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}


void MainWindow::on_textEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}


void MainWindow::on_actionFontColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if(color.isValid()){
        QPalette p = ui->textEdit->palette();
        p.setColor(QPalette::Text, color);
        ui->textEdit->setPalette(p);
    }
}


void MainWindow::on_actionFontBackgroundColor_triggered()
{

}


void MainWindow::on_actionEditColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if(color.isValid()){
        QPalette p = ui->textEdit->palette();
        p.setColor(QPalette::Base, color);
        ui->textEdit->setPalette(p);
    }
}

void MainWindow::on_actionLineWrap_triggered()
{
    QPlainTextEdit::LineWrapMode mode = ui->textEdit->lineWrapMode();

    if(mode==QPlainTextEdit::NoWrap){
        ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        ui->actionLineWrap->setChecked(true);
    } else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actionLineWrap->setChecked(false);
    }
}



void MainWindow::on_actionFont_triggered()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok,this);

    if(ok){
        ui->textEdit->setFont(font);
    }
}


void MainWindow::on_actionToolbar_triggered()
{
    bool visible = ui->toolBar->isVisible();
    ui->toolBar->setVisible(!visible);
    ui->actionToolbar->setChecked(!visible);
}


void MainWindow::on_actionStatusbar_triggered()
{
    bool visible = ui->statusbar->isVisible();
    ui->statusbar->setVisible(!visible);
    ui->actionStatusbar->setChecked(!visible);
}


void MainWindow::on_actionSelectAll_triggered()
{
    ui->textEdit->selectAll();
}


void MainWindow::on_actionExit_triggered()
{
    if(userEditConfirmed()){
        close();
    }
}


void MainWindow::on_textEdit_cursorPositionChanged()
{
    int col = 0;
    int ln = 0;
    int flg = -1;
    int pos = ui->textEdit->textCursor().position();
    QString text = ui->textEdit->toPlainText();

    for(int i=0;i<pos;i++){
        if(text[i]=='\n'){
            ln++;
            flg=i;
        }
    }

    flg++;
    col=pos-flg;
     statusCursorLabel.setText("Ln: "+QString::number(ln+1)+"   Col: "+QString::number(col+1));
}


// void MainWindow::on_actionShowLineNumbers_triggered(bool checked)
// {
//     ui->textEdit->hideLineNumberArea(!checked);
// }




