object RurDBGridViewsDlg: TRurDBGridViewsDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Poh'#318'ady'
  ClientHeight = 414
  ClientWidth = 645
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poOwnerFormCenter
  ShowHint = True
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lblZoznam: TLabel
    Left = 8
    Top = 8
    Width = 86
    Height = 13
    Caption = 'Zoznam poh'#318'adov'
  end
  object lblStlpce: TLabel
    Left = 200
    Top = 8
    Width = 120
    Height = 13
    Caption = 'Zoznam v'#353'etk'#253'ch st'#314'pcov'
  end
  object Label1: TLabel
    Left = 200
    Top = 256
    Width = 143
    Height = 13
    Caption = 'Zoznam u'#382#237'vate'#318'sk'#253'ch st'#314'pcov'
  end
  object lblPStlpce: TLabel
    Left = 390
    Top = 8
    Width = 29
    Height = 13
    Caption = 'St'#314'pce'
  end
  object lblShowHelp: TLabel
    Left = 562
    Top = 70
    Width = 57
    Height = 13
    Cursor = crHelp
    Hint = 'Integrovan'#253' manu'#225'l'
    Caption = 'Manu'#225'l (F1)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    Visible = False
    OnClick = lblShowHelpClick
  end
  object btnOk: TButton
    Left = 562
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 6
  end
  object btnCancel: TButton
    Left = 562
    Top = 39
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Zru'#353'i'#357
    ModalResult = 2
    TabOrder = 7
  end
  object lbZoznam: TListBox
    Left = 8
    Top = 27
    Width = 186
    Height = 206
    ItemHeight = 13
    TabOrder = 0
    OnClick = lbZoznamClick
  end
  object lbStlpce: TListBox
    Left = 200
    Top = 27
    Width = 153
    Height = 206
    ItemHeight = 13
    TabOrder = 1
    OnDblClick = aSRightExecute
  end
  object lbUser: TListBox
    Left = 200
    Top = 275
    Width = 153
    Height = 134
    ItemHeight = 13
    TabOrder = 3
    OnDblClick = aURightExecute
  end
  object lbPStlpce: TListBox
    Left = 390
    Top = 27
    Width = 153
    Height = 382
    ItemHeight = 13
    TabOrder = 5
  end
  object btnSRight: TButton
    Left = 359
    Top = 27
    Width = 25
    Height = 25
    Action = aSRight
    TabOrder = 2
  end
  object btnURight: TButton
    Left = 359
    Top = 275
    Width = 25
    Height = 25
    Action = aURight
    TabOrder = 4
  end
  object btnViewAdd: TButton
    Left = 8
    Top = 275
    Width = 186
    Height = 25
    Action = aViewAdd
    TabOrder = 8
  end
  object btnViewRename: TButton
    Left = 8
    Top = 306
    Width = 186
    Height = 25
    Action = aViewRename
    TabOrder = 9
  end
  object btnViewDelete: TButton
    Left = 8
    Top = 337
    Width = 186
    Height = 25
    Action = aViewDelete
    TabOrder = 10
  end
  object btnMoveUp: TButton
    Left = 562
    Top = 322
    Width = 75
    Height = 25
    Action = aMoveUp
    Caption = 'Posu'#328' hore'
    TabOrder = 11
  end
  object btnMoveDown: TButton
    Left = 562
    Top = 353
    Width = 75
    Height = 25
    Action = aMoveDown
    TabOrder = 12
  end
  object btnDelete: TButton
    Left = 562
    Top = 384
    Width = 75
    Height = 25
    Action = aDelete
    TabOrder = 13
  end
  object actlst1: TActionList
    Left = 584
    Top = 96
    object aViewAdd: TAction
      Caption = 'Kop'#237'rova'#357' nov'#253' poh'#318'ad...'
      Hint = 
        'Ozna'#269'te zdrojov'#253' poh'#318'ad, ktor'#253' sa pou'#382#237'je na skop'#237'rovanie st'#314'pco' +
        'v'
      OnExecute = aViewAddExecute
    end
    object aViewRename: TAction
      Caption = 'Premenova'#357' poh'#318'ad...'
      Hint = 'Premenova'#357' m'#244#382'ete iba svoje poh'#318'ady'
      OnExecute = aViewRenameExecute
      OnUpdate = aViewRenameUpdate
    end
    object aViewDelete: TAction
      Caption = 'Vymaza'#357' poh'#318'ad'
      Hint = 'Vymaza'#357' m'#244#382'ete iba svoje poh'#318'ady'
      OnExecute = aViewDeleteExecute
      OnUpdate = aViewDeleteUpdate
    end
    object aSRight: TAction
      Caption = '>>'
      Hint = 'Prida'#357' st'#314'pec'
      OnExecute = aSRightExecute
      OnUpdate = aSRightUpdate
    end
    object aURight: TAction
      Caption = '>>'
      Hint = 'Prida'#357' st'#314'pec'
      OnExecute = aURightExecute
      OnUpdate = aURightUpdate
    end
    object aMoveUp: TAction
      Caption = 'aMoveUp'
      Hint = 'Posun'#250#357' ozna'#269'en'#253' st'#314'pec vy'#353#353'ie'
      OnExecute = aMoveUpExecute
      OnUpdate = aMoveUpUpdate
    end
    object aMoveDown: TAction
      Caption = 'Posu'#328' dolu'
      Hint = 'Posun'#250#357' ozna'#269'en'#253' st'#314'pec ni'#382#353'ie'
      OnExecute = aMoveDownExecute
      OnUpdate = aMoveDownUpdate
    end
    object aDelete: TAction
      Caption = 'Vymaza'#357
      Hint = 'Vymaza'#357' ozna'#269'en'#253' st'#314'pec'
      OnExecute = aDeleteExecute
      OnUpdate = aDeleteUpdate
    end
  end
end
