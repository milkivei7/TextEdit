#include "textedit.h"
#include "findwords.h"
#include "./ui_textedit.h"

TextEdit::TextEdit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextEdit)
{
    ui   -> setupUi    (this);
    this -> setMenuBar (createMenuBar());

    textEdit     = new QTextEdit (this);
    findClass    = new FindWords;
    replaceClass = new ReplaceWords;

    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);


    setCentralWidget         (textEdit);

    connect(actionAbout,    &QAction::triggered, this, &TextEdit::aboutSlot);
    connect(actionFileOpen, &QAction::triggered, this, &TextEdit::functionFileOpen);
    connect(actionFileSave, &QAction::triggered, this, &TextEdit::functionFileSave);
    connect(actionFileExit, &QAction::triggered, this, &TextEdit::functionFileExit);

    connect(actionEditFind,    &QAction::triggered, this, &TextEdit::functionEditFind);
    connect(actionEditFont,    &QAction::triggered, this, &TextEdit::functionEditFont);
    connect(actionEditReplace, &QAction::triggered, this, &TextEdit::functionEditReplace);
    connect(actionEditCut,     &QAction::triggered, this, &TextEdit::functionEditCut);

    connect(textEdit,          &TextEdit::customContextMenuRequested, this, &TextEdit::showContextMenu);
    connect(actionContextFont,     &QAction::triggered, this, &TextEdit::functionContextEditFont);
}

TextEdit::~TextEdit()
{
    delete ui;
}

void TextEdit::aboutSlot()
{
    QMessageBox::information (0, "Message", "Not implemented");
}

void TextEdit::functionFileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr,  "Выберите приложение",
                                                   QString(), "Исполняемые файлы (*.txt *.doc *.docx)");
    QFile   file(fileName);
    if (fileName.isEmpty()) return;

    if (file.open (QIODevice::ReadOnly | QIODevice::Text)){
        textEdit -> setText (file.readAll());
    }
    else qDebug()<<"Не робит"<<Qt::endl;
}

void TextEdit::functionFileSave()
{
    QString nameFile = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                        tr("Исполняемые файлы (*.txt);;Docx Files (*.doc *.docx);;Pdf (*.pdf)"));
        if (!nameFile.isEmpty())//nameFile != ""
        {
            QFile     file       (nameFile);
            QFileInfo fileInfo   (nameFile);
            QString   suffixFile (fileInfo.completeSuffix());

            qDebug()<<suffixFile<<" Суффикс"<<Qt::endl;

            if (file.open(QIODevice::ReadWrite))
            {
                /*QTextStream stream (&file);
                            stream << textEdit->toPlainText();*/
                if (suffixFile == "pdf"){
                      QPrinter printer;
                      printer.setOutputFormat   (QPrinter::PdfFormat);
                      printer.setOutputFileName (nameFile);

                      QTextDocument *doc = textEdit->document();
                      doc->print(&printer);

                      qDebug()<<"Файл сохранился в PDF"<<Qt::endl;

                }
                if (suffixFile == "txt"||suffixFile =="docx"||suffixFile=="doc"){
                    QTextDocumentWriter textWriter;
                    textWriter.setFileName (file.fileName());
                    textWriter.setFormat   (suffixFile.toLatin1());
                    textWriter.write       (textEdit->document());

                    qDebug()<<"Файл сохранился в TXT"<<Qt::endl;
                }
                file.flush();
                file.close();
            }
            else
            {
                QMessageBox::critical(this, tr("Error"),
                                      tr("Нет файла"));
                return;
            }
        }
        else qDebug()<<"Error"<<Qt::endl;
}

void TextEdit::functionFileExit()
{
    QCoreApplication::quit();
}

void TextEdit::functionEditFind()
{
    QTextDocument* doc = textEdit->document();
    findClass->show();
    findClass->document = doc;
    emit signalTextDocument(doc);
}

void TextEdit::functionEditFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Helvetica[Cronyx]"), this);
    if (ok){
        textEdit->setFont(font);
    }
}

void TextEdit::functionEditReplace()
{
    QTextDocument* doc = textEdit -> document();
    replaceClass -> show();
    replaceClass -> document = doc;
    emit signalTextDocumentForReplace(doc);
}

void TextEdit::functionEditCut()
{
    textEdit->cut();
}

void TextEdit::showContextMenu(const QPoint &pt)
{
    QMenu *menu = textEdit->createStandardContextMenu();

    menu -> addAction (actionEditFind);
    menu -> addAction (actionEditReplace);
    menu -> addAction (actionEditCut);
    menu -> addAction (actionContextFont);

    menu -> exec(QCursor::pos());
    delete menu;
}

void TextEdit::functionContextEditFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Helvetica[Cronyx]"), this);
    if (ok){
        QTextCharFormat format;
        format.setFont(font);
        textEdit->mergeCurrentCharFormat(format);
    }
}

