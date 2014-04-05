/*
    This file is part of Icecream.

    Copyright (c) 2003 Frerich Raabe <raabe@kde.org>
    Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef ICEMON_STARVIEW_H
#define ICEMON_STARVIEW_H

#include "job.h"
#include "statusview.h"

#include <QGraphicsView>
#include <QResizeEvent>
#include <QLabel>
#include <QGraphicsEllipseItem>
#include <QDialog>

class HostInfo;
class StarView;

class QSlider;
class QLabel;
class QLineEdit;
class QCheckBox;
class QGraphicsView;

class StarViewConfigDialog : public QDialog
{
    Q_OBJECT

public:
    StarViewConfigDialog( QWidget *parent );

    int nodesPerRing();
    bool suppressDomainName() const;

    void setMaxNodes( int );

    QString archFilter();

protected slots:
    void slotNodesPerRingChanged( int nodes );
    void slotSuppressDomainName( bool );

signals:
    void configChanged();

private:
    QSlider *mNodesPerRingSlider;
    QLabel *mNodesPerRingLabel;
    QLineEdit *mArchFilterEdit;
    QCheckBox *mSuppressDomainName;
};


class HostItem : public QGraphicsItemGroup
{
public:
    enum { RttiHostItem = 1000 };

    HostItem(const QString &text);
    HostItem( HostInfo *hostInfo, HostInfoManager * );
    ~HostItem();

    void init();

    HostInfo *hostInfo() const { return mHostInfo; }

    void setHostColor( const QColor &color );

    void setIsActiveClient( bool active ) { mIsActiveClient = active; }
    bool isActiveClient() const { return mIsActiveClient; }

    void setIsCompiling( bool compiling ) { mIsCompiling = compiling; }
    bool isCompiling() const { return mIsCompiling; }

    void setStateItem( QGraphicsItem *item ) { m_stateItem = item; }
    QGraphicsItem *stateItem() { return m_stateItem; }

    void setClient( unsigned int client ) { m_client = client; }
    unsigned int client() const { return m_client; }

    QString hostName() const;
    void updateName();

    double centerPosX() const { return pos().x()+m_textItem->boundingRect().width()/2; }
    double centerPosY() const { return pos().y()+m_textItem->boundingRect().height()/2; }

    double relativeCenterPosX() const { return m_textItem->boundingRect().width()/2; }
    double relativeCenterPosY() const { return m_textItem->boundingRect().height()/2; }

    void setCenterPos( double x, double y );

    void update( const Job &job );

protected:
    void createJobHalo( const Job & );
    void deleteJobHalo( const Job &job );
    void updateHalos();

private:
    HostInfo *mHostInfo;
    HostInfoManager *mHostInfoManager;

    bool mIsActiveClient;
    bool mIsCompiling;

    QGraphicsItem *m_stateItem;
    QGraphicsSimpleTextItem* m_textItem;
    unsigned int m_client;

    qreal mBaseWidth;
    qreal mBaseHeight;

    QGraphicsEllipseItem *m_boxItem;

    QMap<Job, QGraphicsEllipseItem *> m_jobHalos;

    JobList m_jobs;
};

class StarViewGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    StarViewGraphicsView(QGraphicsScene* scene, StarView* starView, QWidget* parent = 0);

    void arrangeItems();
    void drawNodeStatus();

protected:
    virtual void resizeEvent( QResizeEvent *e );
    virtual bool event(QEvent *event);

private:
    void arrangeHostItems();
    void centerSchedulerItem();
    void drawState(HostItem *node);

    StarView* m_starView;
    HostItem *m_schedulerItem;
};

class StarView : public StatusView
{
    Q_OBJECT

public:
    StarView( QObject* parent );

    void update( const Job &job );
    virtual QWidget* widget() const;

    QString id() const { return "star"; }

    QList<HostItem*> hostItems() const;
    HostItem *findHostItem( unsigned int hostid );

    virtual void setMonitor(Monitor* monitor);

    void checkNode( unsigned int hostid );

    void removeNode( unsigned int hostid );

    void updateSchedulerState( bool online );

    void configureView();

    bool isConfigurable() { return true; }

    StarViewConfigDialog* configDialog() const;

    /**
       Return true if node should be shown and false if not.
    */
    bool filterArch( unsigned int hostid );
    /**
       Return true if node should be shown and false if not.
    */
    bool filterArch( HostInfo * );

    void removeItem( HostItem * );
    void forceRemoveNode( unsigned int hostid );

protected slots:
    void slotConfigChanged();

private:
    void createKnownHosts();
    HostItem *createHostItem( unsigned int hostid );

    QGraphicsScene* m_canvas;
    QScopedPointer<StarViewGraphicsView> m_widget;
    StarViewConfigDialog *mConfigDialog;

    QMap<unsigned int,HostItem *> m_hostItems;
    QMap<unsigned int,HostItem *> mJobMap;
};

#endif
// vim:ts=4:sw=4:noet
