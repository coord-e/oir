# oir

[![Actions Status](https://github.com/coord-e/oir/workflows/latest/badge.svg)](https://github.com/coord-e/oir/actions?workflow=latest)

OtakuAssembly Vol.2 第一章『データフロー最適化の実装』サンプル実装

| OtakuAssembly Vol.2 | OtakuAssembly Vol.2 電子版 |
| :---: | :---: |
| [![book](https://img.shields.io/badge/get%20it%20on-BOOTH-fc4e50)](https://otakuassembly.booth.pm/items/1834753) | [![ebook](https://img.shields.io/badge/get%20it%20on-BOOTH-fc4e50)](https://otakuassembly.booth.pm/items/1834745) |

## 遊び方

```
usage: oir [options]

    -i input_file           (default: "-")
    -f input_format         (default: "text")
    -o output_file          (default: "-")
    -g output_format        (default: "text")
    -p passes               (default: "reach,prop,available,cse,live,dce")
    -l number_of_loops      (default: "1")
    -h help                 (default: "false")
```

`oir`は入力のOIRに解析や最適化を施して出力するプログラムです。`-o`と`-i`でそれぞれ出力と入力ファイルを指定できます。指定しない場合は標準入出力がデフォルトで使われます。

`-p`オプションで入力に施す解析や最適化とその順番をカンマ区切りで指定できます。さらに、`-l`オプションで`-p`で指定した手順を指定回数繰り返させることができます。

`-g`オプションで出力の形式を指定できます。`-g text`がデフォルトの形式です。`-g json`で、解析結果を含めたOIRについてのほとんどの情報をJSON形式で出力します。`-g graph`では、[Graphviz](https://www.graphviz.org/)のdot形式でOIRのCFGを出力します。出力したdot形式のファイルは、`dot`コマンドでPDFや画像として見ることができます。

例えば、`text/count.oir`にデッドコード削除を行った結果のOIRのCFGを`cfg.pdf`に出力するには、次のようにします:

```shell
$ ./build/oir -i text/count.oir -g graph -p live,dce -o cfg.gv
$ dot -Tpdf -o cfg.pdf cfg.gv
```

## テスト

```shell
$ make test.docker
```

## 報告について

OtakuAssembly Vol.2 第一章『データフロー最適化の実装』について、何か問題や誤植などを見つけた場合は当リポジトリのIssueにて対応いたします。正誤表はこのREADMEに掲載します。その他質問などございましたら、筆者[coord_e](https://twitter.com/coord_e)までお気軽にご連絡ください。
