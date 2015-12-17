Compression/Decompression Software Noah

## 注意事項

ソースはこちら
http://www.kmonos.net/lib/noah.ja.html

私は作者ではありませんが自由に使ってよいライセンスだったのでソースの特定の部分を示しやすいGithubに公開しました。

## 以下 Readme.txt 原文

このソース群の利用条件については、Noahのバイナリの方に
つけてある利用条件に従います。基本的には、どう使うのも自由。
Visual C++ 6.0 Professional Edition / Visual C++ 2010 Express Edition 用です。

### ワークスペース等

- Noah.sln			(VC++ 2010 用ワークスペース)
- Noah.dsw			(VC++ 6.0 用ワークスペース)
   - Noah.dsp		('Noah.exe' 用プロジェクト)
   - stdafx.h/cpp	(pre-complied header生成用)
   - NoahXt/
     - NoahXt.dsp	('NoahXt.dll' 用プロジェクト)
     - NoahXt.def	(ExportするAPI一覧)
     - stdafx.h/cpp	(pre-complied header生成用)
   - uninst/
     - kinst.dsp	('uninst.exe' 用プロジェクト)
     - stdafx.h/cpp	(pre-complied header生成用)


### リソース

   - Noah.rc		(リソーススクリプト)
   - resource.h		(リソースID定義ヘッダ)
   - *.ico			(アイコンデータ)


### ソースコード

 - /
   - NoahApp.h|Noah.cpp	(Noahメインルーチン)
   - NoahCM.h|cpp		(設定関係全般)
   - NoahAM.h|cpp		(圧縮解凍処理の上位ルーチン)
     - SubDlg.h|cpp		(書庫内容閲覧/パスワード/状況表示ダイアログ)
     - Archiver.h|cpp	(圧縮解凍ルーチンのinterface)
       - ArcDLL.h|cpp	(DLL利用ルーチン)
       - ArcB2e.h|cpp	(B2Eスクリプト利用ルーチン)
       - ArcACE.h|cpp	(UnaceV2.dll利用ルーチン)
       - ArcMSC.h|cpp	(MS-Compress対応ルーチン)
       - ArcCPT.h|cpp	(CompactPro対応ルーチン)

 - NoahXt/
    - NoahXt.cpp	(関連付け・シェルエクステンション)

 - uninst/
    - kinst.cpp		(インストーラ)

 - kilib/
   - kilib.h		(K.I.LIBのメインヘッダ)
   - kilibext.h		(K.I.LIBの拡張機能用ヘッダ)

   - kl_app.h|cpp	(スタートアップポイント。アプリケーション全体の情報管理)
   - kl_wnd.h|cpp	(ウインドウ、ダイアログ、プロパティシート管理)
   - kl_reg.h|cpp	(レジストリ、iniファイルの入出力)
   - kl_dnd.h|cpp	(OLEドラッグ＆ドロップ処理)
   - kl_find.h|cpp	(ファイル検索)
   - kl_wcmn.h|cpp	(主にWindows-Shell周りの便利関数群)

   - kl_cmd.h|cpp	(コマンドラインパーサ)
   - kl_str.h|cpp	(文字列処理＆パス文字列処理)
   - kl_file.h|cpp	(ファイル入出力)
   - kl_misc.h|cpp	(汎用クラス。配列のtemplateなど)

   - kl_carc.h|cpp	(統合アーカイバDLLラッパ)
   - kl_rythp.h|cpp	(Rythpスクリプト処理)


### 構成

- CNoahApp : kiApp
  - (ArcManやCnfManと連絡を取り、行う処理を決定する処理)

  - CNoahArchiveManager
	 - (適切なCArchiverへ圧縮解凍を回す処理)
	 - kiArray<CArchiver*>

  - CNoahConfigManager
	 - (ini読み書き処理やNoahXt.dllとの連携)
	 - CNoahConfigDialog : kiPropSheet
	   - CCmprPage : kiPropSheetPage
	   - CInfoPage : kiPropSheetPage
	   - CMeltPage : kiPropSheetPage
	   - CWinXPage : kiPropSheetPage

- CArchiver
	 - (書庫操作用の共通インターフェイス)
	 - (ここから各種解凍エンジン専用クラスが派生する)

- K.I.LIB
	 - (汎用性の極めて低いWin32用ライブラリ)











