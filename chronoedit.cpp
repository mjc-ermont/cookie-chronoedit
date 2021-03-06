#include "chronoedit.h"
#include "ui_chronoedit.h"

ChronoEdit::ChronoEdit(QWidget *parent) : QWidget(parent), ui(new Ui::ChronoEdit){
    ui->setupUi(this);

    ui->events->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    open();

    connect(ui->addContributeur, SIGNAL(pressed()), this, SLOT(ajouterContrib()));
    connect(ui->addContributeur, SIGNAL(pressed()), ui->setContributeur, SLOT(clear()));

    connect(ui->addLieu, SIGNAL(pressed()), this, SLOT(ajouterLieu()));
    connect(ui->addLieu, SIGNAL(pressed()), ui->setLieu, SLOT(clear()));

    connect(this, SIGNAL(notAllSet(bool)), ui->addEvent, SLOT(setEnabled(bool)));
    connect(ui->checkContribs, SIGNAL(pressed()), this, SLOT(refreshContribLabel()));
    connect(this, SIGNAL(deletePressed()), this, SLOT(deleteEvent()));
    connect(ui->deleteContribs, SIGNAL(pressed()), this, SLOT(deleteSelectedContrib()));

    connect(ui->addLieu, SIGNAL(pressed()), this, SLOT(verifyEnable()));
    connect(ui->eventTitle, SIGNAL(textEdited(QString)), this, SLOT(verifyEnable()));
    connect(ui->checkContribs, SIGNAL(pressed()), this, SLOT(verifyEnable()));
    connect(ui->addEvent, SIGNAL(pressed()), this, SLOT(ajouterEvent()));
    connect(ui->deleteLieu, SIGNAL(pressed()), this, SLOT(deleteSelectedLieu()));

    connect(ui->addLieu, SIGNAL(pressed()), this, SLOT(save()));
    connect(ui->addContributeur, SIGNAL(pressed()), this, SLOT(save()));
    connect(ui->addEvent, SIGNAL(pressed()), this, SLOT(save()));
    connect(ui->events, SIGNAL(cellChanged(int, int)), this, SLOT(save(int, int)));
}

ChronoEdit::~ChronoEdit(){
    delete ui;
}

QString ChronoEdit::json(){
    QJsonDocument dJson;
    QJsonObject json;

        QJsonArray contributeurs;
        for(int i = 0 ; i < ui->Contributeurs->count() ; i++){
            contributeurs.append( ui->Contributeurs->item(i)->text() );
        }
        json.insert("contributeurs", contributeurs);

        QJsonArray lieux;
        for(int i = 0 ; i < ui->Lieux->count() ; i++){
            lieux.append( ui->Lieux->item(i)->text() );
        }
        json.insert("lieux", lieux);

        QJsonArray events;
        for(int i = 0 ; i < ui->events->rowCount() ; i ++){
            QJsonObject event;
                event.insert("titre", ui->events->item(i, 0)->text());
                event.insert("debut", ui->events->item(i, 1)->text());
                event.insert("avant", ui->events->item(i, 2)->text());
                event.insert("fin", ui->events->item(i, 3)->text());
                event.insert("lieu", ui->events->item(i, 4)->text());
                event.insert("contributeurs", ui->events->item(i, 5)->text());
                event.insert("description", ui->events->item(i, 6)->text());
            events.append(event);
        }
        json.insert("events", events);

    dJson.setObject(json);
    return QString( dJson.toJson() );
}

void ChronoEdit::setContrib(QString contrib){
    ui->Contributeurs->addItem(contrib);
}

void ChronoEdit::setLieu(QString lieu){
    ui->Lieux->addItem(lieu);
    ui->eventPlace->addItem(lieu);
}

void ChronoEdit::setEvent(QString titre, QString debut, QString fin, QString lieu, QString resp, bool avant, QString desc){
    QString moment;
    if(avant)
        moment = "avant";
    else
        moment = "apres";
    int ligne = ui->events->rowCount();

    ui->events->insertRow(ligne);
    ui->events->setItem(ligne, 0, new QTableWidgetItem(titre));
    ui->events->setItem(ligne, 1, new QTableWidgetItem(debut));
    ui->events->setItem(ligne, 2, new QTableWidgetItem(moment));
    ui->events->setItem(ligne, 3, new QTableWidgetItem(fin));
    ui->events->setItem(ligne, 4, new QTableWidgetItem(lieu));
    ui->events->setItem(ligne, 5, new QTableWidgetItem(resp));
    ui->events->setItem(ligne, 6, new QTableWidgetItem(desc));

    refreshContribLabel();
}

void ChronoEdit::ajouterContrib(){
    if(ui->Contributeurs->findItems(ui->setContributeur->text(), Qt::MatchExactly).count()   ||   ui->setContributeur->text().isEmpty())
        return;

    setContrib(ui->setContributeur->text());
}

void ChronoEdit::ajouterLieu(){
    if(ui->Lieux->findItems(ui->setLieu->text(), Qt::MatchExactly).count()   ||    ui->setLieu->text().isEmpty())
        return;

    setLieu(ui->setLieu->text());
}

void ChronoEdit::ajouterEvent(){
    setEvent(ui->eventTitle->text(), ui->debut->text(), ui->fin->text(), ui->eventPlace->itemText(ui->eventPlace->currentIndex()), ui->eventResp->text(), ui->avant->isChecked(), ui->eventDesc->toPlainText());

    ui->eventTitle->clear();
    ui->eventDesc->clear();
}

void ChronoEdit::save(int a, int b){
    if(a != -1 && b != -1)
        qDebug() << "["<<a<<"-"<<b<<"] change";

    QFile fichier("chronologie.json");
    QString config( json() );

    if(fichier.open(QIODevice::WriteOnly))
    {
        qDebug("Ouverture du fichier de configuration pour ecriture reussie");

        fichier.write( config.toUtf8() );

        fichier.flush();
        fichier.close();
    }
}

void ChronoEdit::open(){
    QFile fichier("chronologie.json");
    QString json("");

    // Lecture du fichier
    if(fichier.open(QIODevice::ReadOnly)){
        qDebug("\nOuverture du fichier de configuration pour lecture reussie");
        json = fichier.readAll();
        fichier.flush();
        fichier.close();
    }

    // Parsage du Json
    QJsonDocument d = QJsonDocument::fromJson(json.toUtf8());
    if(d.isNull())  qWarning() << "Json invalide";
    QJsonObject j = d.object();

    // Exploitation
    foreach(QJsonValue contrib, j["contributeurs"].toArray()){
        setContrib(contrib.toString());
    }
    foreach(QJsonValue lieu, j["lieux"].toArray()){
        setLieu(lieu.toString());
    }
    foreach(QJsonValue event, j["events"].toArray()){
        QJsonObject e = event.toObject();
        setEvent(e["titre"].toString(),
                 e["debut"].toString(),
                 e["fin"].toString(),
                 e["lieu"].toString(),
                 e["contributeurs"].toString(),
                 e["avant"].toString() == "avant",
                 e["description"].toString() );
    }
}

void ChronoEdit::refreshContribLabel(){
    QList<QListWidgetItem *> liste( ui->Contributeurs->selectedItems() );
    QString text("");

    foreach( QListWidgetItem *i , liste){
        text += i->text() + ", ";
    }
    text.resize(text.size() - 2);
    if( liste.size() == 0 ){
        text = "<span style='color:#ff0000 ; font-weight:bold'> Aucun - selectionnez dans l'onglet \"Contributeurs\"</span>";
    }

    ui->eventResp->setText(text);
}

void ChronoEdit::verifyEnable(){
    bool checker (true);

    if(ui->eventTitle->text().isEmpty())
        checker = false;
    if(ui->Lieux->count() == 0)
        checker = false;
    if(ui->eventResp->text() == "Aucun - selectionnez dans l'onglet \"Contributeurs\"")
        checker = false;

    emit notAllSet(checker);
}

void ChronoEdit::deleteEvent(){
    if(ui->tabWidget->currentIndex() == 0){
        QList<int> lignes;
        lignes << -1;
        foreach(QTableWidgetItem* i, ui->events->selectedItems()){
            if(!lignes.contains(i->row())){
                int j(0);
                while(lignes[j] > i->row()){
                    j++;
                }
                lignes.insert(j, i->row());
            }
        }

        foreach(int i, lignes){
            ui->events->removeRow(i);
        }
        save();
    }

    if(ui->tabWidget->currentIndex() == 2){
        deleteSelectedContrib();
    }

    if(ui->tabWidget->currentIndex() == 3){
        deleteSelectedLieu();
    }
}

void ChronoEdit::keyPressEvent(QKeyEvent *e){
    if(e->key() == Qt::Key_Delete){
        emit deletePressed();
    }
}

void ChronoEdit::deleteSelectedContrib(){
    qDeleteAll(ui->Contributeurs->selectedItems());
    save();
}

void ChronoEdit::deleteSelectedLieu(){
    qDeleteAll(ui->Lieux->selectedItems());
    save();
}
