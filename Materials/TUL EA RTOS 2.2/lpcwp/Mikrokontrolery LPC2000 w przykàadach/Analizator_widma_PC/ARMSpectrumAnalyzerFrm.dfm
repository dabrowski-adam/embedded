object frmARMSpectrumAnalyzer: TfrmARMSpectrumAnalyzer
  Left = 195
  Top = 116
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'frmARMSpectrumAnalyzer'
  ClientHeight = 351
  ClientWidth = 545
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnMouseMove = FormMouseMove
  PixelsPerInch = 96
  TextHeight = 13
  object speedbtnBkgColors: TSpeedButton
    Left = 16
    Top = 312
    Width = 23
    Height = 22
    Caption = 'B'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    OnClick = BtnColorsClick
  end
  object speedbtnLinesColor: TSpeedButton
    Left = 40
    Top = 312
    Width = 23
    Height = 22
    Caption = 'L'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    OnClick = BtnColorsClick
  end
  object speedbtnRun: TSpeedButton
    Left = 80
    Top = 312
    Width = 121
    Height = 25
    AllowAllUp = True
    GroupIndex = 1
    Caption = 'Run'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    OnClick = speedbtnRunClick
  end
  object pnlFFT: TPanel
    Left = 16
    Top = 16
    Width = 514
    Height = 258
    BevelOuter = bvLowered
    Color = clSkyBlue
    TabOrder = 0
    object paintboxFFT: TPaintBox
      Left = 1
      Top = 1
      Width = 512
      Height = 256
      Cursor = crCross
      OnMouseDown = paintboxFFTMouseDown
      OnMouseMove = paintboxFFTMouseMove
      OnPaint = paintboxFFTPaint
    end
  end
  object pnlXY: TPanel
    Left = 16
    Top = 280
    Width = 513
    Height = 25
    BevelOuter = bvLowered
    TabOrder = 1
    object lbY: TLabel
      Left = 102
      Top = 4
      Width = 19
      Height = 16
      Caption = 'Y: '
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
    end
    object lbX: TLabel
      Left = 7
      Top = 4
      Width = 18
      Height = 16
      Caption = 'X: '
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
    end
    object lbF: TLabel
      Left = 197
      Top = 4
      Width = 18
      Height = 16
      Caption = 'F: '
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
    end
    object lbV: TLabel
      Left = 292
      Top = 4
      Width = 19
      Height = 16
      Caption = 'V: '
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
    end
  end
  object pnlWindowType: TPanel
    Left = 216
    Top = 312
    Width = 153
    Height = 25
    BevelOuter = bvLowered
    Caption = 'Window: None'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
  end
  object colordialogColors: TColorDialog
    Ctl3D = True
    Left = 16
    Top = 248
  end
end
