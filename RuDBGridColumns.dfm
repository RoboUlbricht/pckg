object RurDBGridColumnsDlg: TRurDBGridColumnsDlg
  Left = 274
  Top = 195
  BorderStyle = bsDialog
  Caption = 'Nastavenie st'#314'pcov'
  ClientHeight = 226
  ClientWidth = 328
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  ShowHint = True
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OKBtn: TButton
    Left = 244
    Top = 8
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKBtnClick
  end
  object CancelBtn: TButton
    Left = 244
    Top = 38
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Zru'#353'i'#357
    ModalResult = 2
    TabOrder = 1
  end
  object lb: TCheckListBox
    Left = 8
    Top = 8
    Width = 225
    Height = 209
    ItemHeight = 13
    TabOrder = 2
  end
  object Button1: TButton
    Left = 244
    Top = 120
    Width = 75
    Height = 25
    Caption = 'Roztiahnu'#357
    Enabled = False
    TabOrder = 3
    OnClick = Roztiahnut
  end
  object Button2: TButton
    Left = 244
    Top = 160
    Width = 75
    Height = 25
    Hint = 'Posun'#250#357' ozna'#269'en'#253' st'#314'pec hore'
    Caption = 'Hore'
    TabOrder = 4
    OnClick = Hore
  end
  object Button3: TButton
    Left = 244
    Top = 192
    Width = 75
    Height = 25
    Hint = 'Posun'#250#357' ozna'#269'en'#253' st'#314'pec dole'
    Caption = 'Dolu'
    TabOrder = 5
    OnClick = Dolu
  end
end
