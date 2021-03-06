/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "dropdown.h"

class MediaView : public TWidget, public RPCSender, public Animated {
	Q_OBJECT

public:

	MediaView();

	void paintEvent(QPaintEvent *e);
	
	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void contextMenuEvent(QContextMenuEvent *e);
	void touchEvent(QTouchEvent *e);

	bool event(QEvent *e);

	void hide();

	void updateOver(QPoint mpos);

	void showPhoto(PhotoData *photo, HistoryItem *context);
	void showPhoto(PhotoData *photo, PeerData *context);
	void showDocument(DocumentData *doc, HistoryItem *context);
	void moveToScreen();
	void moveToNext(int32 delta);
	void preloadData(int32 delta);

	void leaveToChildEvent(QEvent *e) { // e -- from enterEvent() of child TWidget
		updateOverState(OverNone);
	}
	void enterFromChildEvent(QEvent *e) { // e -- from leaveEvent() of child TWidget
		updateOver(mapFromGlobal(QCursor::pos()));
	}

	void mediaOverviewUpdated(PeerData *peer, MediaOverviewType type);
	void documentUpdated(DocumentData *doc);
	void changingMsgId(HistoryItem *row, MsgId newId);
	void updateDocSize();
	void updateControls();
	void updateDropdown();

	bool animStep(float64 dt);

	void showSaveMsgFile();
	void close();

	void activateControls();
	void onDocClick();

	~MediaView();

public slots:

	void onHideControls(bool force = false);
	void onDropdownHiding();

	void onToMessage();
	void onSaveAs();
	void onDownload();
	void onSaveCancel();
	void onShowInFolder();
	void onForward();
	void onDelete();
	void onOverview();
	void onCopy();
	void onMenuDestroy(QObject *obj);
	void receiveMouse();

	void onDropdown();

	void onCheckActive();
	void onTouchTimer();

	void updateImage();
	void onGifUpdated();

private:

	void displayPhoto(PhotoData *photo, HistoryItem *item);
	void displayDocument(DocumentData *doc, HistoryItem *item);
	void findCurrent();
	void loadBack();

	void photosLoaded(History *h, const MTPmessages_Messages &msgs, mtpRequestId req);
	void userPhotosLoaded(UserData *u, const MTPphotos_Photos &photos, mtpRequestId req);
	void filesLoaded(History *h, const MTPmessages_Messages &msgs, mtpRequestId req);

	void updateHeader();
	void snapXY();

	QBrush _transparentBrush;

	PhotoData *_photo;
	DocumentData *_doc;
	MediaOverviewType _overview;
	QRect _closeNav, _closeNavIcon;
	QRect _leftNav, _leftNavIcon, _rightNav, _rightNavIcon;
	QRect _headerNav, _nameNav, _dateNav;
	QRect _saveNav, _saveNavIcon, _moreNav, _moreNavIcon;
	bool _leftNavVisible, _rightNavVisible, _saveVisible, _headerHasLink;
	QString _dateText;
	QString _headerText;

	Text _caption;
	QRect _captionRect;

	uint64 _animStarted;

	int32 _width, _x, _y, _w, _h, _xStart, _yStart;
	int32 _zoom; // < 0 - out, 0 - none, > 0 - in
	float64 _zoomToScreen; // for documents
	QPoint _mStart;
	bool _pressed;
	int32 _dragging;
	QPixmap _current;
	AnimatedGif _currentGif;
	int32 _full; // -1 - thumb, 0 - medium, 1 - full

	style::sprite _docIcon;
	style::color _docIconColor;
	QString _docName, _docSize, _docExt;
	int32 _docNameWidth, _docSizeWidth, _docExtWidth;
	QRect _docRect, _docIconRect;
	int32 _docThumbx, _docThumby, _docThumbw;
	uint64 _docRadialFirst, _docRadialStart, _docRadialLast;
	float64 _docRadialOpacity;
	QPen _docRadialPen;
	anim::fvalue a_docRadial, a_docRadialStart;
	LinkButton _docDownload, _docSaveAs, _docCancel;

	History *_history; // if conversation photos or files overview
	PeerData *_peer;
	UserData *_user; // if user profile photos overview
	
	UserData *_from;
	Text _fromName;

	int32 _index; // index in photos or files array, -1 if just photo
	MsgId _msgid; // msgId of current photo or file

	mtpRequestId _loadRequest;

	enum OverState {
		OverNone,
		OverLeftNav,
		OverRightNav,
		OverClose,
		OverHeader,
		OverName,
		OverDate,
		OverSave,
		OverMore,
		OverIcon,
	};
	OverState _over, _down;
	QPoint _lastAction, _lastMouseMovePos;
	bool _ignoringDropdown;

	enum ControlsState {
		ControlsShowing,
		ControlsShown,
		ControlsHiding,
		ControlsHidden,
	};
	ControlsState _controlsState;
	uint64 _controlsAnimStarted;
	QTimer _controlsHideTimer;
	anim::fvalue a_cOpacity;

	ContextMenu *_menu;
	Dropdown _dropdown;
	IconedButton *_btnSaveCancel, *_btnToMessage, *_btnShowInFolder, *_btnSaveAs, *_btnCopy, *_btnForward, *_btnDelete, *_btnViewAll;
	QList<IconedButton*> _btns;

	bool _receiveMouse;

	bool _touchPress, _touchMove, _touchRightButton;
	QTimer _touchTimer;
	QPoint _touchStart;
	QPoint _accumScroll;

	QString _saveMsgFilename;
	uint64 _saveMsgStarted;
	anim::fvalue _saveMsgOpacity;
	QRect _saveMsg;
	QTimer _saveMsgUpdater;
	Text _saveMsgText;

	typedef QMap<OverState, uint64> Showing;
	Showing _animations;
	typedef QMap<OverState, anim::fvalue> ShowingOpacities;
	ShowingOpacities _animOpacities;

	void updateOverRect(OverState state);
	bool updateOverState(OverState newState);
	float64 overLevel(OverState control);
	QColor overColor(const QColor &a, float64 ca, const QColor &b, float64 cb);

};
