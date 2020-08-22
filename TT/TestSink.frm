VERSION 5.00
Object = "{CFA1C5A4-D1B5-4590-8515-380DB1E7E92C}#1.0#0"; "TestSink.dll"
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox Text1 
      Height          =   375
      Left            =   480
      TabIndex        =   1
      Text            =   "Text1"
      Top             =   480
      Width           =   1095
   End
   Begin TESTSINKLibCtl.TestEvntSink TestEvntSink2 
      Height          =   735
      Left            =   2280
      OleObjectBlob   =   "TestSink.frx":0000
      TabIndex        =   0
      Top             =   1920
      Width           =   2055
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub TestEvntSink2_Fire2Event(ByVal nMsg As Long)
    Text1.Text = Str(nMsg)
End Sub
