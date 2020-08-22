VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Test GlueFile"
   ClientHeight    =   3705
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3705
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox Text4 
      Height          =   375
      Left            =   120
      TabIndex        =   7
      Text            =   "Text4"
      Top             =   1680
      Width           =   1455
   End
   Begin VB.CommandButton Command1 
      Caption         =   "FN2 Conv"
      Height          =   375
      Left            =   120
      TabIndex        =   6
      Top             =   3240
      Width           =   1215
   End
   Begin VB.TextBox Text3 
      Height          =   375
      Left            =   840
      TabIndex        =   4
      Text            =   "Text3"
      Top             =   1080
      Width           =   3735
   End
   Begin VB.TextBox Text2 
      Height          =   375
      Left            =   840
      TabIndex        =   2
      Text            =   "Text2"
      Top             =   600
      Width           =   3735
   End
   Begin VB.TextBox Text1 
      Height          =   375
      Left            =   840
      TabIndex        =   0
      Text            =   "Text1"
      Top             =   120
      Width           =   3735
   End
   Begin VB.Label Label3 
      Caption         =   "Sign"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   1080
      Width           =   615
   End
   Begin VB.Label Label2 
      Caption         =   "Dest"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   600
      Width           =   615
   End
   Begin VB.Label Label1 
      Caption         =   "Source:"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   615
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
 Dim srcfile As String
 Dim dstfile As String
 Dim signfile As String
 Dim strfields As String
 Dim ret As Integer
 Dim ss As String
 
 srcfile = Text1.Text
 dstfile = Text2.Text
 signfile = Text3.Text
 strfields = "PatFirst=John|PatLast=Qing|RefrDrLast=Smith, M.D.|Document=01234567|Dob=01/20/2001|TranDT=01/20/2001 06:00|TEST"
 
 Set objFile = CreateObject("GlueFiles.CreFnetFile.1")
 If IsObject(objFile) = True Then
   objFile.CreateViewFile srcfile, dstfile, strfields, 1, 1, "7777", "123456", 0
   Text4.Text = objFile.ValidSignProc(dstfile, signfile, 0)
 End If
 Set objFile = Nothing

End Sub

Private Sub Form_Load()
    Text1.Text = "C:\TEMP\111.fn2"
    Text2.Text = "C:\TEMP\111s.fn2"
End Sub
