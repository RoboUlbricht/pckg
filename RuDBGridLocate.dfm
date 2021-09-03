object LocateDlg: TLocateDlg
  Left = 462
  Top = 209
  BorderStyle = bsDialog
  Caption = 'Vyh'#318'adanie v st'#314'pci'
  ClientHeight = 60
  ClientWidth = 255
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  ShowHint = True
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 29
    Height = 13
    Caption = 'St'#314'pec'
  end
  object Label2: TLabel
    Left = 8
    Top = 32
    Width = 49
    Height = 13
    Caption = 'Obsahuje '
  end
  object OKBtn: TButton
    Left = 212
    Top = 32
    Width = 37
    Height = 21
    Hint = 'Potvrdenie vyh'#318'adania'
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object c: TComboBox
    Left = 64
    Top = 8
    Width = 145
    Height = 21
    Hint = 'Vyberte st'#314'pec'
    Style = csDropDownList
    DropDownCount = 20
    TabOrder = 0
    OnChange = cChange
  end
  object e: TEdit
    Left = 64
    Top = 32
    Width = 145
    Height = 21
    Hint = 'Text za'#269#237'naj'#250'ci na'
    TabOrder = 1
    OnChange = eChange
  end
  object Button1: TButton
    Left = 280
    Top = 8
    Width = 35
    Height = 25
    Cancel = True
    Caption = 'Button1'
    ModalResult = 2
    TabOrder = 3
  end
end
