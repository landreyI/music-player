#ifndef QMUSIC_H
#define QMUSIC_H


#include <QMessageBox>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QToolButton>
#include <QFile>
class QMusic : public QWidget
{
    Q_OBJECT
public:
    QMusic(QString path1,QWidget *parent=nullptr);
    QMusic(const QMusic& other);
    ~QMusic();

    QString getName(){return name->text();}
    QString getArtist(){return artist->text();}
    QString getAlbum(){return album;}
    int getYear(){return year;}
    QString getGenre(){return genre;}
    QString getPath(){return path;}

    void setName(QString text){name->setText(text);}
    void setArtist(QString text){artist->setText(text);}
    void setAlbum(QString album){this->album=album;}
    void setYear(int year){this->year=year;}
    void setGenre(QString genre){this->genre=genre;}

    QMusic& operator=(const QMusic& other);
private:
    QLabel *name;
    QLabel *artist;
    QString album;
    int year;
    QString genre;
    QString path;
    void pushTool(QHBoxLayout *hlayout);
private slots:

protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (event->type() == QEvent::MouseButtonPress) {
            emit path_(this);
        }
        return QWidget::eventFilter(object, event);
    }
signals:
    void path_(QMusic *mus);
    void delet(QMusic *mus);
    void add_playlist(QMusic *mus);
    void inf_mus(QMusic *mus);
};
#endif // QMUSIC_H

