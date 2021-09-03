object DatumDlg: TDatumDlg
  Left = 227
  Top = 108
  BorderStyle = bsDialog
  Caption = 'D'#225'tum'
  ClientHeight = 139
  ClientWidth = 242
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poScreenCenter
  ShowHint = True
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OKBtn: TButton
    Left = 160
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Dnes'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKBtnClick
  end
  object CancelBtn: TButton
    Left = 160
    Top = 102
    Width = 75
    Height = 25
    Hint = 'Vr'#225'ti'#357' akciu sp'#228#357' bez zmeny'
    Cancel = True
    Caption = 'Zru'#353'i'#357
    ModalResult = 2
    TabOrder = 1
  end
  object ZBtn: TButton
    Left = 160
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Zajtra'
    TabOrder = 2
    OnClick = ZBtnClick
  end
end
