#ifndef CHRONOEDIT_H
#define CHRONOEDIT_H

#include <QtGui>
#if QT_VERSION >= 0x050000
    #include <QtWidgets>
#endif

// Define pour deux fonctions modifies avec Qt5
#if QT_VERSION >= 0x050000
    #define TO_ASCII(str) str.toLatin1()
    #define SET_RESIZE_MODE(mode) ui->events->horizontalHeader()->setSectionResizeMode(mode)
#else
    #define TO_ASCII(str) str.toAscii()
    #define SET_RESIZE_MODE(mode) ui->events->horizontalHeader()->setResizeMode(mode)
#endif

namespace Ui {
class ChronoEdit;
}

class ChronoEdit : public QWidget
{
    Q_OBJECT
    
    public:
        explicit ChronoEdit(QWidget *parent = 0);
        ~ChronoEdit();

        QString json();

        void setContrib(QString contrib);
        void setLieu(QString lieu);
        void setEvent(QString titre, QString debut, QString fin, QString lieu, QString resp, bool avant = 1, QString desc = "");

    private:
        Ui::ChronoEdit *ui;

    protected:
        void keyPressEvent(QKeyEvent * e);

    private slots:
        void ajouterContrib();
        void ajouterLieu();
        void ajouterEvent();

        void verifyEnable();
        void refreshContribLabel();

        void deleteEvent();

        void deleteSelectedContrib();
        void deleteSelectedLieu();

    public slots:
        void save();
        void open();

    signals:
        void notAllSet(bool a = false);
        void deletePressed();
};

#endif // CHRONOEDIT_H
