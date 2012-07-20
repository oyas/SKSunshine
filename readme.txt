2011/7

スーパーくねくねサンシャイン。
文化祭出展作品。
もとはLinuxで動く3DゲームをLinuxで作るのが目的だった。
ので、このゲームはLinux(動作確認はUbuntu)でも動く。
ただし、freeglut,OpenAL,glpngのインストールが必要。


 === ゲーム説明 ===

プログラム本体は、
Ubuntuの場合、「a.out」
Windowsの場合、「a.exe」

ゲームの目的は奥へ進みクリアマスの上に乗ることです。
５回ステージの下へ落ちるとMISSとなりゲーム終了です。

◆操作方法◆

space	・・・ ジャンプ
a,s	・・・ カメラ横回転
w,z	・・・ カメラ縦回転
q,e	・・・ カメラ距離調整
←,2	･･･ 左に移動
→,4	･･･ 右に移動
↑,5	･･･ 前に移動
↓,6	･･･ 後に移動



 === ディレクトリ構造 ===

src ・・・　ソースコードが入ってる
  XMash ・・・　Xファイル読み込み系
  Collision ・・・　あたり判定系
  Stage ・・・　ステージ。ゲームの内容とかここ。
  Sound ・・・　OpenAL関連。BGM担当。
bin ・・・　実行ファイルが入ってる。makeすると自動でここに生成。中のdllはWindowsで動作するために必要。
Makefile ・・・　そのまま
a.out ・・・　bin/a.outを実行するシェルスクリプト。
readme ・・・　これ。
test_project.geany ・・・　Geanyのプロジェクトファイル。



 === make方法 ===
Ubuntu  :　$ make
Windows :　$ make win



 === 参考にしたページ ===
覚えている限り記録。

GLUTによる「手抜き」OpenGL入門
http://www.wakayama-u.ac.jp/~tokoi/opengl/libglut.html

Project Asura
http://asura.iaigiri.com/
(Xファイル読み込み http://asura.iaigiri.com/OpenGL/gl44.html)

OpenGL入門
http://wisdom.sakura.ne.jp/system/opengl/index.html

OpenGL de プログラミング
http://wiki.livedoor.jp/mikk_ni3_92/

ゲームプログラマーを目指すひと
http://rudora7.blog81.fc2.com/blog-entry-310.html

tontonのHP OpenAL
http://island.geocities.jp/v_no11/programing/OpenAL.html

青空の果て OpenGL(GLUT) ～PNG for glpng～
http://www.web-sky.org/program/opengl/opengl01.html


 === 音楽をかりたとこ ===
フリーBGM　Music with myuu
http://www.ne.jp/asahi/music/myuu/wave/wave.htm

