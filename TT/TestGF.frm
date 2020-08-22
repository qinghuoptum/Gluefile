VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   4305
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   6585
   LinkTopic       =   "Form1"
   ScaleHeight     =   4305
   ScaleWidth      =   6585
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command9 
      Caption         =   "W-R"
      Height          =   435
      Left            =   120
      TabIndex        =   19
      Top             =   3720
      Width           =   675
   End
   Begin VB.CommandButton Command8 
      Caption         =   "2"
      Height          =   375
      Left            =   6120
      TabIndex        =   18
      Top             =   2040
      Width           =   255
   End
   Begin VB.CommandButton Command7 
      Caption         =   "1"
      Height          =   375
      Left            =   5760
      TabIndex        =   17
      Top             =   2040
      Width           =   255
   End
   Begin VB.CommandButton Command6 
      Caption         =   "0"
      Height          =   375
      Left            =   5400
      TabIndex        =   16
      Top             =   2040
      Width           =   255
   End
   Begin VB.CommandButton Command4 
      Caption         =   "Test"
      Height          =   375
      Left            =   5400
      TabIndex        =   15
      Top             =   1440
      Width           =   495
   End
   Begin VB.TextBox Text6 
      Height          =   375
      Left            =   6120
      TabIndex        =   14
      Text            =   "0"
      Top             =   2880
      Width           =   375
   End
   Begin VB.TextBox Text5 
      Height          =   375
      Left            =   1920
      TabIndex        =   13
      Text            =   "Patient=|DOB=|"
      Top             =   3360
      Width           =   4575
   End
   Begin VB.TextBox Text4 
      Height          =   375
      Left            =   1920
      TabIndex        =   11
      Text            =   "C:\WORKENV\ALEX\LDINH.RTF"
      Top             =   2880
      Width           =   3255
   End
   Begin VB.TextBox Text3 
      Height          =   375
      Left            =   1920
      TabIndex        =   8
      Top             =   2400
      Width           =   3255
   End
   Begin VB.TextBox Text2 
      Height          =   375
      Left            =   1920
      TabIndex        =   7
      Top             =   1920
      Width           =   3255
   End
   Begin VB.TextBox Text1 
      Height          =   375
      Left            =   1920
      TabIndex        =   4
      Text            =   "C:\WORKENV\ALEX\09926641s.fn2"
      Top             =   1440
      Width           =   3255
   End
   Begin VB.CommandButton Command5 
      Caption         =   "FnetUtls"
      Height          =   855
      Left            =   3480
      TabIndex        =   3
      Top             =   360
      Width           =   1215
   End
   Begin VB.CommandButton Command3 
      Caption         =   "InterMed"
      Height          =   855
      Left            =   1920
      TabIndex        =   2
      Top             =   360
      Width           =   1335
   End
   Begin VB.CommandButton Command2 
      Caption         =   "VOC -> WAVE"
      Height          =   855
      Left            =   4800
      TabIndex        =   1
      Top             =   360
      Width           =   1455
   End
   Begin VB.CommandButton Command1 
      Caption         =   "FN2 File Convert"
      Height          =   855
      Left            =   240
      TabIndex        =   0
      Top             =   360
      Width           =   1455
   End
   Begin VB.Label Re 
      Caption         =   "Replace Str"
      Height          =   375
      Left            =   240
      TabIndex        =   12
      Top             =   3360
      Width           =   1455
   End
   Begin VB.Label Label4 
      Caption         =   "Signature File"
      Height          =   375
      Left            =   240
      TabIndex        =   10
      Top             =   2880
      Width           =   1335
   End
   Begin VB.Label Label3 
      Caption         =   "Unglued File (RTF)"
      Height          =   375
      Left            =   240
      TabIndex        =   9
      Top             =   2400
      Width           =   1455
   End
   Begin VB.Label Label2 
      Caption         =   "Destine File (FN2)"
      Height          =   375
      Left            =   240
      TabIndex        =   6
      Top             =   1920
      Width           =   1575
   End
   Begin VB.Label Label1 
      Caption         =   "Source File (RTF)"
      Height          =   255
      Left            =   240
      TabIndex        =   5
      Top             =   1440
      Width           =   1455
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim gObj As Object

Private Sub Command1_Click()
 Dim srcfile As String
 Dim dstfile As String
 Dim signfile As String
 Dim resfile As String
 Dim strfields As String
 Dim res As String
 Dim ret As Integer
 
 'srcfile = "\\DR-FNTMAIN\D\WFS\0001\rpt00000\00007002.fn2"
 srcfile = Text1.Text
 dstfile = Text2.Text
 savfile = Text3.Text
 sigfile = Text4.Text
 strfields = Text5.Text
 batch = CInt(Text6.Text)
 
 dstfile = "d:\00003178s.rtf"
 
 savfile = "d:\00003178s.fn2"
 'savfile = "H:\TEMP\6002s.lst"
 'savfile2 = "H:\TEMP\6002t.fn2"
 'sigfile = "D:\WORKENV\ALEX\PIC\ABRUNE.RTF"
 sigfile = "*This is a text sign line QSDW"
 strfields = "Security=1|FileList=8|RefrDrLast=YSmith, M.D.|Document=01234567|Dob=01/20/2001|TranDT=01/20/2001 06:00|JobNo=08012346"
 'strfields = "PatFirst=John|Dob=01/20/1975|Diag=9/3/2001 Test Diag USER : Wang xiangzy -------------------- |Treat=|Purpose=9/3/2001 Test purpose USER : Wang xiangzy -------------------- |Medic=9/3/2001 No Med USER : Wang xiangzy -------------------- |Notes=|SendDt=09/05/2001 |ResendDt=|FollowUp=|CsltComm=9/28/2001 DKA=3 -------------------- |RChoice=Return|Dka=3|MCancel=NO|CCancel=NO|PtRefuse=N|RcvDt=|AcpDt=|RtnDt=|FwdDt=|BkDt=|BkTm=|PrgCode=|OrdLoc=Bakersfield|MedCal=Non Medicare|OtherSpcl=080 :GASTROENTEROLOGY "
 
 'srcfile = "D:\TEMP\LabelTmplt.rtf"
 'dstfile = "D:\TEMP\9999.rtf"
 
 Set objFile = CreateObject("GlueFiles.CreFnetFile.1")
 If IsObject(objFile) = True Then
   
    'If srcfile <> "" Then
      'res = objFile.CreateViewFile(srcfile, dstfile, strfields, 1, 1, "7777", "123456", batch)
      'objFile.CreateViewFile srcfile, dstfile, strfields, 1, 1, "7777", "123456", batch
    'End If
    
    If sigfile <> "" Then
      res = objFile.TranslateToSignfile(dstfile, savfile, sigfile, 1, 0, 0, "7777", "123456", 0)
    End If
    
    'If savfile <> "" Then
      'objFile.Save dstfile, savfile2
    'End If
    
    'objFile.Save resfile, savfile
    'ret = objFile.TranslateToSignfile(dstfile, savfile, sigfile, 3, 0, 0, "7777", "123456", 0)
    'objFile.CreateViewFile srcfile, dstfile, strfields, 0, 0, "7777", "123456"
   
    'objFile.GetFilename srcfile, dstfile
    'objFile.RefreshField strfields
    'InsertSignPic signfile, 1
   
    'objFile.LabelGenerate dstfile, srcfile, strfields
   
 End If
 Set objFile = Nothing

End Sub

Private Sub Command2_Click()
 Dim srcfile As String
 Dim dstfile As String
 Dim signfile As String
 Dim resfile As String
 Dim strfields As String
 Dim ret As Integer
 
 srcfile = "D:\WORKENV\CTest.pcm"
 dstfile = "D:\TEMP\0008.wav"
  
 Set objFile = CreateObject("GlueFiles.CreFnetFile.1")
 If IsObject(objFile) = True Then
   objFile.Vox2WaveFile srcfile, dstfile
 End If
 Set objFile = Nothing

End Sub

Private Sub Command3_Click()
 Dim strBuf As String
 Dim ret As Integer
 
 strBuf = ""
 Set objFile = CreateObject("InterMed.IseriesInt.1")
 If IsObject(objFile) = True Then
   ret = objFile.Initialize("FNETDEV4", "Fnet", "78621", "0001", "Genedbs", "FNETDEV4")
   
   n = Val(Text1.Text)
   strBuf = ""
   ret = objFile.GetMultiRpts("227", strBuf, 500, n)
   MsgBox (strBuf)
   
   'strBuf = ""
   'ret = objFile.LoadOneRept("00206070", strBuf, 1000)
   'MsgBox (strBuf)
   'strBuf = ""
   ret = objFile.SignOneReptEx("03631739", "227", "", strBuf)
   MsgBox (strBuf)
   
   'ret = objFile.SignOneReptEx("00052128", "152", "", strBuf)
   'MsgBox (strBuf)
   
   'ret = objFile.SignBatchReptEx("D:\TEMP\LIST.TXT", strBuf)
   'MsgBox (strBuf)
   
   ret = objFile.Close()
 End If
 Set objFile = Nothing

End Sub

Private Sub Command4_Click()
 Dim srcfile As String
 Dim dstfile As String
 Dim signfile As String
 Dim strfields As String
 
 srcfile = "D:\TEMP\Test001.rtf"
 dstfile = "D:\TEMP\Test001.htm"
 'signfile = "D:\TEMP\karr.rtf"
 'signfile = "D:\TEMP\Pictures\ABarre.rtf"
 'strfields = "Document=02129412|JobNo=12801349|RptCode=0030|DictDT=Jun 27 2001 |DictDrCode=4494 |DictDrFirst=URI|DictDrLast=BEN-ZUR, M.D.|PatFirst=HOWARD1|PatLast=PERSKY1|PatientL=PERSKY1,HOWARD1|Dob=01/01/1900 |Sex=Male|Marriage= |Age= |SSN=|MrNo=00141688 |CaseNo=0425785 |VisitNo=|MedicalNo=|RoomBed=|AdmtDT=June 27, 2001 |SrveDT=June 27, 2001 |DschDT=December 31, 1969 |AtndDrCode= |RefrDrCode= |AdmtDrCode= |TranCode=5059 |TranDT=June 27, 2001 10:00AM "
 ' "PatientL=John Smith|MrNo=122345654|JobNo=54321|RefrDrFirst=John|SrveDT=01/20/2001|TranDT=01/20/2001 06:00|TEST"
  
 Set objFile = CreateObject("GlueFiles.CreFnetFile.1")
 If IsObject(objFile) = True Then
   strfields = objFile.ConvertFile(srcfile, dstfile, 1, "")
   Text4.Text = strfields
   'objFile.InsertSignPic signfile, 1
   'objFile.GetFilename srcfile, dstfile
 End If
 Set objFile = Nothing
End Sub

Private Sub Command5_Click()
 
 Dim srcfile As String
 srcfile = "FTP://QINGHU/TestDoc.txt"
 srcfile = "HTTP://QINGHU/Fnet2000/TestDoc.txt"
 'srcfile = "D:\TEMP\TestDoc.txt"
 Set objFile = CreateObject("FnetUtls.FnetUtlsDoc.1")
 If IsObject(objFile) = True Then
   objFile.GetFileList srcfile
   ' ScanFileList Options:
   ' 1 - Display List
   ' 2 - Display each file (text format)
   ' 3 - Use Word to print each file
   ' 4 - Download each file
   objFile.ScanFileList 1
   objFile.ScanFileList 2
   'objFile.ScanFileList 4
 End If
 Set objFile = Nothing

End Sub

Private Sub Command6_Click()

Set gObj = CreateObject("GlueFiles.CreFnetFile.1")
End Sub

Private Sub Command7_Click()
 Dim srcfile As String
 Dim dstfile As String
 Dim signfile As String
 Dim strfields As String
 
 srcfile = "D:\TEMP\Test001.rtf"
 dstfile = "D:\TEMP\Test001.htm"
 If IsObject(gObj) = True Then
   strfields = gObj.ConvertFile(srcfile, dstfile, 1, "")
   Text4.Text = strfields
   'gObj.CreateHtmlFile "", "", ""
   'objFile.InsertSignPic signfile, 1
   'objFile.GetFilename srcfile, dstfile
 End If
End Sub

Private Sub Command8_Click()
  Set gObj = Nothing
End Sub

Private Sub Command9_Click()
 Dim srcfile As String
 Dim dstfile As String
 Dim signfile As String
 Dim strfields As String
 
 Set gObj = CreateObject("GlueFiles.CreFnetFile.1")
 srcfile = "D:\TEMP\rocky-swartz2.Doc"
 dstfile = "D:\TEMP\Test001.rf"
 If IsObject(gObj) = True Then
   strfields = gObj.CreateRTFFile(srcfile, dstfile, 4, "")
   Text4.Text = strfields
   'gObj.CreateHtmlFile "", "", ""
   'objFile.InsertSignPic signfile, 1
   'objFile.GetFilename srcfile, dstfile
 End If
 Set gObj = Nothing
End Sub
