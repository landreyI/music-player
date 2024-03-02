#ifndef QPLAYLIST_H
#define QPLAYLIST_H

#include "QMusic.h"
#include <QStackedWidget>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QToolButton>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QTextStream>
#include <fstream>
class QPlaylist : public QWidget
{
    Q_OBJECT
public:
    QPlaylist(QString path, QVector<QMusic*>vec_playlist, QString name_str, QWidget *parent=nullptr);

    QString getName(){return name->text();}
    void setName(QString text){name->setText(text);}

    QVBoxLayout* getLayout(){return list_mus;}

    QVector<QMusic*> getVec(){return musics_playlist;}
    void change_photo(QPixmap pixmap);
    void add_music(QMusic *mus);
    void del_music(QMusic *mus);
private:
    QVBoxLayout *list_mus;
    QWidget *mainWidget;
    QWidget *musicList_widget;
    QString path_namePlaylist;
    QLabel *quantity_mus;
    QLabel *picture;
    QLabel *name;

    QVector<QMusic*> musics_playlist;
    void add_setting(QHBoxLayout *layout,QLabel *name_p);

private slots:
    void hide_playlist();
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (event->type() == QEvent::MouseButtonPress) {
            mainWidget->hide();
            musicList_widget->show();
            emit hide(this);
        }
        return QWidget::eventFilter(object, event);
    }
signals:
    void hide(QPlaylist *show_);
    void show_(QPlaylist *hide_);
    void delet(QPlaylist *del);
};
#endif // QPLAYLIST_H
