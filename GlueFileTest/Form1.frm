VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   5370
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   7170
   LinkTopic       =   "Form1"
   ScaleHeight     =   5370
   ScaleWidth      =   7170
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox batchid 
      Height          =   375
      Left            =   1680
      TabIndex        =   7
      Text            =   "4361"
      Top             =   4320
      Width           =   1695
   End
   Begin VB.TextBox txtTmp 
      Height          =   375
      Left            =   1680
      TabIndex        =   6
      Text            =   "d:\2.rtf"
      Top             =   3600
      Width           =   5415
   End
   Begin VB.TextBox txtWFS 
      Height          =   375
      Left            =   1800
      TabIndex        =   3
      Text            =   "d:\1.fn2"
      Top             =   2760
      Width           =   5295
   End
   Begin VB.CommandButton Command3 
      Caption         =   "TestSaveRTFFile"
      Height          =   615
      Left            =   2400
      TabIndex        =   2
      Top             =   1800
      Width           =   1695
   End
   Begin VB.CommandButton Command2 
      Caption         =   "TestCreateRTFView"
      Height          =   615
      Left            =   2400
      TabIndex        =   1
      Top             =   960
      Width           =   1695
   End
   Begin VB.CommandButton Command1 
      Caption         =   "TranslateToSignFile"
      Height          =   495
      Left            =   2400
      TabIndex        =   0
      Top             =   240
      Width           =   1695
   End
   Begin VB.Label Label2 
      Caption         =   "Temp File"
      Height          =   375
      Left            =   0
      TabIndex        =   5
      Top             =   3720
      Width           =   1455
   End
   Begin VB.Label Label1 
      Caption         =   "WFS File and Path"
      Height          =   375
      Left            =   0
      TabIndex        =   4
      Top             =   2760
      Width           =   1575
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
        Dim ObjFile As Object
        Set ObjFile = CreateObject("GlueFiles.CreFnetFile.1")
        ObjFile.TranslateToSignFile "d:\00003178s.rtf", "c:\inetpub\wwwroot\chart\temp\00003178.rtf", "*Electronically signed by Baker,Nance at 8/21/2006 9:5:33", 1, 0, 0, "7777", "123456", 0
        Set ObjFile = Nothing
End Sub

Private Sub Command2_Click()
        Dim ObjFile As Object
        Set ObjFile = CreateObject("GlueFiles.CreFnetFile.1")
        ObjFile.CreateViewFile txtWFS.Text, txtTmp.Text, _
        "Document=09613180|JobNo=00168863|RptCode=0003|DictDT=10 20 2005|DictDrCode=0290|DictDrFirst=SAJID|DictDrLast=ALI, M.D.|PatFirst=SONIA|PatLast=OTERO|Dob=01/01/1900|Sex=|Marriage=|Age=|SSN=|MrNo=M00133581|CaseNo=I009365636|VisitNo=|MedicalNo=|RoomBed=I.0156|AdmtDT=10 16 2005 1|SrveDT=10 20 2005 1|DschDT=|AtndDrCode=0290|AtndDrFirst=SAJID|AtndDrLast=ALI, M.D.|DictDTitle=|Speciality=PSYCH|AtndDTitle=|AtndSpeciality=PSYCH|RefrDrCode=|RefrDrFirst=|RefrDrLast=|AdmtDrCode=|AdmtDrFirst=|AdmtDrLast=|TranCode=8888|TranDT=10 21 2005 10:59AM|Security=1|DraftLine=1", _
        1, 1, "6666", "123456", CInt(batchid.Text)
        
        Set ObjFile = Nothing
End Sub

Private Sub Command3_Click()
        Dim ObjFile As Object
        Set ObjFile = CreateObject("GlueFiles.CreFnetFile.1")
        ObjFile.Save txtTmp.Text, txtWFS.Text
        Set ObjFile = Nothing
End Sub
