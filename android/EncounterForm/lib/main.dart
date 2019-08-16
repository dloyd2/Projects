import 'package:flutter/material.dart';
import 'package:gradient_app_bar/gradient_app_bar.dart';

import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';

import 'package:mailer/mailer.dart';
import 'package:mailer/smtp_server.dart';

import 'package:google_sign_in/google_sign_in.dart';
import 'package:googleapis_auth/auth_io.dart';
import 'package:googleapis/sheets/v4.dart' as sheet;

final _credentials = new ServiceAccountCredentials.fromJson(r'''
{
  "type": "service_account",
  "project_id": "__REMOVED__",
  "private_key_id": "__REMOVED__",
  "private_key": "__REMOVED__",
  "client_email": "__REMOVED__",
  "client_id": "__REMOVED__",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "__REMOVED__"
}
''');
const _scopes = const [sheet.SheetsApi.SpreadsheetsScope];
const sheetId = "__REMOVED__";
const sheetUrl = "https://docs.google.com/spreadsheets/d/" + sheetId;

const mailRecipient = "__REMOVED__";
const mailFrom = {
  "name": "__REMOVED__",
  "email": "__REMOVED__",
  "password": "__REMOVED__"
};

const debug = false;

final GoogleSignIn _googleSignIn = GoogleSignIn();
final FirebaseAuth _auth = FirebaseAuth.instance;

Color lightBlue = Color.fromARGB(255, 0, 166, 207);
Color darkBlue = Color.fromARGB(255, 54, 73, 155);
TextStyle headerStyle =
    TextStyle(fontSize: 20.0, fontWeight: FontWeight.bold, color: darkBlue);
Text Header(str) {
  return Text(
    str,
    style: headerStyle,
  );
}

FirebaseUser user;

void _logout() {
  _auth.signOut();
  _googleSignIn.signOut();
  user = null;
}

void log(msg) {
  if (debug) {
    print("LOG: " + msg);
  }
}

void main() {
  runApp(MyApp());
}

String getDate() {
  //Return the date as a string in mm/dd/yyyy hh:mm format
  DateTime date = DateTime.now();
  String min =
      date.minute < 10 ? '0' + date.minute.toString() : date.minute.toString();
  String hour =
      date.hour < 10 ? '0' + date.hour.toString() : date.hour.toString();
  String month =
      date.month < 10 ? '0' + date.month.toString() : date.month.toString();
  String day = date.day < 10 ? '0' + date.day.toString() : date.day.toString();

  String formattedDate = month + "-" + day + "-" + date.year.toString();
  formattedDate += " " + hour + ":" + min;
  return formattedDate;
}

Future<bool> send_to_sheet_and_mail(info) async
// info is an array of strings, in the order desired by the sheet
// sends info into the sheet and sends an email to the global variable mailRecipient
{
  bool success = true;
  log('getting oauth');

  await clientViaServiceAccount(_credentials, _scopes).then((client) {
    log('got oauth');
    sheet.SheetsApi api = new sheet.SheetsApi(client);
    sheet.ValueRange vr = new sheet.ValueRange.fromJson({
      "values": [info]
    });
    log('about to append');
    api.spreadsheets.values
        .append(vr, sheetId, 'A:I', valueInputOption: 'USER_ENTERED')
        .then((sheet.AppendValuesResponse r) {
      log('append completed.');
      client.close();
    }).catchError((e) {
      log(e);
      success = false;
    });

    log('called append()');
  });

  final smtpServer = await gmail(mailFrom["email"], mailFrom["password"]);
  String subjectText = "__REMOVED__";
  String bodyText =
      "__REMOVED__" +
          sheetUrl;

  final message = Message()
    ..from = Address(mailFrom["email"], mailFrom["name"])
    ..recipients.add(mailRecipient)
    ..subject = subjectText
    ..text = bodyText;
  try {
    final sendReport = await send(message, smtpServer);
    log("sent email: " + sendReport.toString());
  } on MailerException catch (e) {
    success = false;
    log("email failed to send");
    log(e);
  }

  return success;
}

String parse(inStr) {
  //data stored from firebase is retrieved and characters replaced are reverted to originals
  String ret = inStr.replaceAll(RegExp(r'\+'), '.');
  ret = ret.replaceAll(RegExp(r'<'), '#');
  ret = ret.replaceAll(RegExp(r'>'), '\$');
  ret = ret.replaceAll(RegExp(r'&'), '[');
  ret = ret.replaceAll(RegExp(r'='), ']');
  ret = ret.replaceAll(RegExp(r'_'), '/');
  return ret;
}

String getFirstWord(phrase) {
  //returns the first word in a string
  List<String> words = phrase.split(" ");
  return words[0];
}

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(title: 'Sign In', home: SignIn());
  }
}

class SignInState extends State<SignIn> {
  String signin_email = "";
  String signin_pass = "";

  Future<void> _logInPopUp() async {
    Text titleText = Text("Please wait");
    Text bodyText = Text('While we verify this email address');

    return showDialog<void>(
      context: context,
      barrierDismissible: false,
      builder: (BuildContext context) {
        return AlertDialog(
          title: titleText,
          content: SingleChildScrollView(
            child: ListBody(
              children: <Widget>[
                bodyText,
              ],
            ),
          ),
        );
      },
    );
  }

  void _signIn_success() {
    //pop the login pop up and push the form
    Navigator.of(context).pop();

    Navigator.push(context, MaterialPageRoute(builder: (BuildContext context) {
      return EncounterForm();
    }));
  }

  Future<void> _signIn_failure() {
    Navigator.of(context).pop();

    log("not verified, signing out");
    _auth.signOut();
    _googleSignIn.signOut();
    user = null;

    Text titleText = Text("Uh oh...");
    Text bodyText = Text(
        'The provided email adress is not a verified user. If this is incorrect, please contact an admin.');
    return showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext context) {
        return AlertDialog(
          title: titleText,
          content: SingleChildScrollView(
            child: ListBody(
              children: <Widget>[
                bodyText,
              ],
            ),
          ),
          actions: <Widget>[
            Container(
              padding: EdgeInsets.fromLTRB(0, 0, 100, 0),
              child: FlatButton(
                child: Text('Okay'),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
            ),
          ],
        );
      },
    );
  }
  Future<void> _signIn_error(err) {
    Navigator.of(context).pop();

    _auth.signOut();
    _googleSignIn.signOut();
    user = null;

    Text titleText = Text("Uh oh...");
    Text bodyText = Text(err);
    return showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext context) {
        return AlertDialog(
          title: titleText,
          content: SingleChildScrollView(
            child: ListBody(
              children: <Widget>[
                bodyText,
              ],
            ),
          ),
          actions: <Widget>[
            Container(
              padding: EdgeInsets.fromLTRB(0, 0, 100, 0),
              child: FlatButton(
                child: Text('Okay'),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
            ),
          ],
        );
      },
    );
  }
  Future<FirebaseUser> _handleSignIn() async {
    final GoogleSignInAccount googleUser = await _googleSignIn.signIn();
    final GoogleSignInAuthentication googleAuth =
        await googleUser.authentication;
    final AuthCredential credential = GoogleAuthProvider.getCredential(
      accessToken: googleAuth.accessToken,
      idToken: googleAuth.idToken,
    );

    user = (await _auth.signInWithCredential(credential)).user;
    _logInPopUp();
    log("verifying ");
    log(user.email);
    bool verified = false;

    DatabaseReference db =
        FirebaseDatabase.instance.reference().child('users/');
    await db.once().then((DataSnapshot snapshot) {
      Map<dynamic, dynamic> values = snapshot.value;
      values.forEach((data, values) {
        if (user.email == parse(data)) {
          log("email verified");
          verified = true;
        }
      });
    });
    // check the list of admins as well if not found in users
    if (!verified) {
      db = FirebaseDatabase.instance.reference().child('admins/');
      await db.once().then((DataSnapshot snapshot) {
        Map<dynamic, dynamic> values = snapshot.value;
        values.forEach((data, values) {
          if (user.email == parse(data)) {
            log("email verified as admin");
            verified = true;
          }
        });
      });
    }

    if (verified) {
      _signIn_success();
    } else {
      _signIn_failure();
    }
    return user;
  }

  @override
  Widget build(BuildContext context) {
    Widget button = Center(
        child: ButtonTheme(
            minWidth: 120.0,
            height: 40.0,
            child: RaisedButton(
                child: Text(
                  "LOGIN",
                  style: TextStyle(color: Colors.white),
                ),
                color: darkBlue,
                shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(30.0)),
                onPressed: () {
                  log("sign in");

                  _handleSignIn().then((user) {
                    log("user signed in");
                  }).catchError((e) => _signIn_error(e));
                })));

    return Scaffold(
        body: Container(
            width: MediaQuery.of(context).size.width,
            height: MediaQuery.of(context).size.height,
            decoration: BoxDecoration(
                gradient: LinearGradient(
                    begin: Alignment.topRight,
                    end: Alignment.bottomLeft,
                    stops: [0.15, 0.88],
                    colors: [lightBlue, darkBlue])),
            child: Column(children: [
              Container(
                padding: EdgeInsets.fromLTRB(0, 50, 0, 0),
                child: Image.asset(
                  "assets/LoginIcon.png",
                  height: 300,
                  width: 250,
                ),
              ),
              Container(
                  width: 300,
                  height: 120,
                  child: Card(
                      margin: EdgeInsets.fromLTRB(15, 0, 15, 0),
                      elevation: 20,
                      shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(8),
                          side: BorderSide(width: 1.0, color: Colors.white)),
                      child: button)),
            ])));
  }
}

class SignIn extends StatefulWidget {
  @override
  SignInState createState() => SignInState();
}

class HelpPage extends StatelessWidget {
  TextStyle helperStyle = TextStyle(fontSize: 18);
  String padding = "    ";
  List<List<String> > elements = [
    ["General",
      "If there is more information than space to show it, each page will expand by becoming scrollable... including this page!",
      "If you cannot find an element that you are looking for, try scrolling to see more.",
      "To maximize security, you will be automatically signed out of your account upon successfully submitting a form. If you must exit the app before completing the form, please sign out manually(top right button)",
    ],
    [
      "Address Selection",
      "The addresses are listed in sorted order. You can scroll to view more addresses if you do not see the address you're looking for.",
      " The addresses can be filtered using the search bar. If no addresses appear, then the search criteria had no results.",
      "Only one address may be chosen at a time. A green checkmark will show which address is currently selected.",
    ],
    [
      "Program Selection",
      "Click on the drop down to see options, then click on the desired program."
    ],
    [
      "Issue Selection",
      "You can scroll to view all common issues. Click on as many issues as reflect the situation.",
      "If no issue provided matches the situation, input the new issue into the bottom textbox as concisely as possible."
    ],
    [
      "Response Selection",
      "You can scroll to view all common responses. Click on as many responses as reflect the situation.",
      "If no response provided matches the situation, input a recommendation into the bottom textbox as concisely as possible."
    ],
    [
      "Status",
      "There are some cases where the issue(s) will require a simple resolution that can be completed during the visit. This dropdown allows for being able to mark these accurately.",
      "NOTE: Still record and submit forms for these encounters. It is important to maintain an accurate list of all issues, regardless of how quickly they can be fixed."
    ],
    [
      "Comments",
      "Input any additional comments into this section as concisely as possible.",
      "Only add comments necessary to address/understand the issue."
    ],
    [
      "Review and Submission",
      "If needed, the window becomes scrollable to fit all info.",
      "Pop up windows will alert you of a successful submission, or if something goes wrong.",
      "Upon a successful submission, you will be logged out of your account.",
      "If an error occurs, try again when you have better connection."
    ],
  ];

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: GradientAppBar(
          backgroundColorStart: lightBlue,
          backgroundColorEnd: darkBlue,
          title: Text("Encounter Form"),
          actions: <Widget>[
            IconButton(
              icon: Icon(
                Icons.keyboard_backspace,
                color: Colors.white,
              ),
              onPressed: () {
                _back(context);
              },
            ),
            IconButton(
              icon: Icon(Icons.exit_to_app),
              onPressed: () {
                _signOut(context);
              },
            ),
          ],
        ),
        body: ListView.builder(
          itemCount: elements.length+2,
          itemBuilder: (context, int){
            if(int == 0){
              return Column(children: <Widget>[
                Padding(
                    padding: EdgeInsets.fromLTRB(0, 15, 0, 0),
                    child: Center(child: Header("Help Page"))),
                Divider(),
              ],);
            }
            if(int == elements.length+1){
              return ButtonTheme(
                  minWidth: 200,
                  height: 50,
                  child: RaisedButton(
                    color: lightBlue,
                    onPressed: () {
                      _back(context);
                    },
                    child: Text(
                      "Back",
                      style: TextStyle(fontSize: 15.0, color: Colors.white),
                    ),
                  ));
            }
            List<String>info = elements[int-1];
            var ctr = 1;
            List<Widget> text = [];
            for(; ctr < info.length; ctr++){
              text.add(Padding(
                  padding: EdgeInsets.fromLTRB(15, 5, 15, 5),
                  child:Text(
                    padding+ctr.toString()+") "+info[ctr], style: helperStyle,
                  )
              ));
            }
            Column sectionText = Column(children: text,);

            Widget section = Column(
              children: <Widget>[
                Padding(
                  padding: EdgeInsets.fromLTRB(5, 0, 0, 0),
                  child:
                Align(
                  alignment: Alignment.centerLeft,
                  child:
                Header(info[0]))),
                sectionText
              ],
            );
            return section;
          },

        ));
  }

  void _back(BuildContext context) {
    Navigator.pop(context);
  }

  void _signOut(BuildContext context) {
    _logout();
    Navigator.pop(context); //go back to form
    Navigator.pop(context); //go back to sign in page
  }
}

class EncounterFormState extends State<EncounterForm> {
  List<String> issues = [];
  Set<String> issuesChecked = Set<String>();
  String otherIssues = "";
  int otherIssuesOffset = 0;

  List<String> responses = [];
  Set<String> responsesChecked = Set<String>();
  String otherResponses = "";
  int otherResponsesOffset = 0;

  List<String> addresses = [];
  List<String> addressChoices = [];

  String addressValue;
  String finalAddressValue = "";
  String addressQuery = "";
  int addressQueryOffset = 0;

  List<String> programs = [];
  String programsValue;
  String finalProgramValue = "";

  List<String> statusOptions = ["Open", "Closed"];
  String status = "Open";

  int numPages = 1;
  int toShow = 0;

  String comments = "";
  int commentsOffset = 0;

  Future<void> _afterConfirm(isSuccessful) async {
    Text titleText = Text("Success!");
    Text bodyText =
        Text('Form submission was successful. You will now be signed out.');
    if (!isSuccessful) {
      titleText = Text("Uh oh!");
      bodyText = Text(
          "Looks like something went wrong. Check your connection and try again.");
    }
    return showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext context) {
        return AlertDialog(
          title: titleText,
          content: SingleChildScrollView(
            child: ListBody(
              children: <Widget>[
                bodyText,
              ],
            ),
          ),
          actions: <Widget>[
            Container(
              padding: EdgeInsets.fromLTRB(0, 0, 100, 0),
              child: FlatButton(
                child: Text('Okay'),
                onPressed: () {
                  Navigator.of(context).pop();
                  if (isSuccessful) {
                    _signOut();
                  }
                },
              ),
            ),
          ],
        );
      },
    );
  }

  Future<void> _waitPopUp() async {
    Text titleText = Text("Please wait");
    Text bodyText = Text(
        'While we ensure the form submission is successful. This should take no more than a few seconds.');

    return showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext context) {
        return AlertDialog(
          title: titleText,
          content: SingleChildScrollView(
            child: ListBody(
              children: <Widget>[
                bodyText,
              ],
            ),
          ),
        );
      },
    );
  }

  void submitForm() async {
    List<String> firstOfResponses = responsesChecked.map(getFirstWord).toList();
    if (otherResponses != "") {
      firstOfResponses.add(otherResponses);
    }
    firstOfResponses.sort();

    List<String> issuesToSend = issuesChecked.toList();
    if (otherIssues != "") {
      issuesToSend.add(otherIssues);
    }
    issuesToSend.sort();
    log("submitted form at:" + getDate());

    List<String> data = [
      finalProgramValue,
      finalAddressValue,
      "",
      issuesToSend.join(", "),
      firstOfResponses.join(", "),
      comments,
      status,
      getDate(),
      user.email
    ];
    Navigator.of(context).pop();
    _waitPopUp();
    bool success = await send_to_sheet_and_mail(data);
    Navigator.of(context).pop();
    _afterConfirm(success);
  }

  Future<void> _confirmSubmit() async {
    return showDialog<void>(
      context: context,
      barrierDismissible: false, // user must tap button!
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text("Submit Confirm"),
          content: SingleChildScrollView(
            child: ListBody(
              children: <Widget>[
                Text('Are you sure you want to submit?'),
              ],
            ),
          ),
          actions: <Widget>[
            Container(
              padding: EdgeInsets.fromLTRB(0, 0, 28, 0),
              child: FlatButton(
                child: Text('Back'),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
            ),
            Container(
              //padding: EdgeInsets.fromLTRB(0, 0, 0, 0),
              child: FlatButton(
                child: Text('Submit'),
                onPressed: () {
                  submitForm();
                },
              ),
            )
          ],
        );
      },
    );
  }

  void _gotoHelpPage() {
    Navigator.push(context, MaterialPageRoute(builder: (BuildContext context) {
      return HelpPage();
    }));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: GradientAppBar(
          backgroundColorStart: lightBlue,
          backgroundColorEnd: darkBlue,
          title: Text("Encounter Form"),
          actions: <Widget>[
            IconButton(
                icon: Icon(Icons.help_outline),
                onPressed: () {
                  log("help");
                  _gotoHelpPage();
                }),
            IconButton(
              icon: Icon(Icons.exit_to_app),
              onPressed: _signOut,
            ),
          ],
        ),
        body: _getBody());
  }

  void _signOut() {
    _logout();
    Navigator.pop(context);
    // push stack on the navigator
  }

  void _searchAddresses() {
    addressChoices.clear();
    int len = addresses.length;
    int i = 0;
    String temp;
    for (; i < len; i++) {
      temp = addresses[i];
      if (temp.contains(RegExp(addressQuery))) {
        addressChoices.add(temp);
      }
    }

    log("new address choices:");
    log(addressChoices.toString());
    addressChoices.sort();
  }

  void _updateData() {
    if (programs.length == 0) {
      List<String> temp = [];
      DatabaseReference db =
          FirebaseDatabase.instance.reference().child('programs/');
      db.once().then((DataSnapshot snapshot) {
        Map<dynamic, dynamic> values = snapshot.value;
        values.forEach((data, values) {
          temp.add(parse(data));
        });
        setState(() {
          programs = temp;
        });
      });
    }
    if (responses.length == 0) {
      List<String> temp = [];
      DatabaseReference db =
          FirebaseDatabase.instance.reference().child('responses/');
      db.once().then((DataSnapshot snapshot) {
        Map<dynamic, dynamic> values = snapshot.value;
        values.forEach((data, values) {
          temp.add(parse(data));
        });
      });
      setState(() {
        responses = temp;
      });
    }
    if (issues.length == 0) {
      List<String> temp = [];
      DatabaseReference db =
          FirebaseDatabase.instance.reference().child('issues/');
      db.once().then((DataSnapshot snapshot) {
        Map<dynamic, dynamic> values = snapshot.value;
        values.forEach((data, values) {
          temp.add(parse(data));
        });
      });
      setState(() {
        issues = temp;
      });
    }
    if (addresses.length == 0) {
      List<String> temp = [];
      DatabaseReference db =
          FirebaseDatabase.instance.reference().child('addresses/');
      db.once().then((DataSnapshot snapshot) {
        Map<dynamic, dynamic> values = snapshot.value;
        values.forEach((data, values) {
          temp.add(parse(data));
        });
        setState(() {
          addresses = temp;
      });
        _searchAddresses();
      });
    }
  }

  Widget _getNavButtons() {
    bool isZero = toShow <= 0;
    bool isAtMax = (toShow == numPages - 1);
    double height = 5;
    Widget backButton = RaisedButton(
      elevation: height,
      onPressed: isZero
          ? null
          : () {
              setState(() {
                toShow = (isZero) ? 0 : (toShow - 1);
              });
            },
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(30.0)),
      child: Text("Back", style: TextStyle(fontSize: 15)),
    );

    String forwardText = isAtMax ? "Submit" : "Forward";
    Widget forwardButton = RaisedButton(
        elevation: height,
        onPressed: isAtMax
            ? () {
                _confirmSubmit();
              }
            : () {
                setState(() {
                  toShow = isAtMax ? numPages - 1 : toShow + 1;
                });
              },
        shape:
            RoundedRectangleBorder(borderRadius: BorderRadius.circular(30.0)),
        child: Text(forwardText, style: TextStyle(fontSize: 15)));
    final double horizWidth = MediaQuery.of(context).size.width / 14;
    Widget ret = ButtonBar(
        mainAxisSize: MainAxisSize.min,
        alignment: MainAxisAlignment.center,
        children: <Widget>[
          backButton,
          Padding(
              padding:
                  EdgeInsets.symmetric(vertical: 0, horizontal: horizWidth)),
          forwardButton,
        ]);
    return ret;
  }

  List<DropdownMenuItem> _getPrograms() {
    List<Widget> ret = programs.map((str) {
      return DropdownMenuItem<String>(value: str, child: Text(str));
    }).toList();
    return ret;
  }

  List<DropdownMenuItem> _getStatusOptions() {
    List<Widget> ret = statusOptions.map((str) {
      return DropdownMenuItem<String>(value: str, child: Text(str));
    }).toList();
    return ret;
  }

  Widget _getBody() {
    _updateData();
    programs.sort();
    issues.sort();
    responses.sort();
    List<DropdownMenuItem> programs_dropdown = _getPrograms();
    TextEditingController textController =
        TextEditingController(text: comments);
    textController.selection = TextSelection.collapsed(offset: commentsOffset);
    TextEditingController addressSearchController =
        TextEditingController(text: addressQuery);
    addressSearchController.selection =
        TextSelection.collapsed(offset: addressQueryOffset);

    Icon chosenIcon = Icon(Icons.check_circle, color: Colors.green);
    Icon notChosenIcon = Icon(Icons.check_circle_outline);

    Widget page1 = Column(
      children: <Widget>[
        Padding(
            padding: EdgeInsets.fromLTRB(0, 15, 0, 0),
            child: Header("Address")),
        Container(
            width: 250,
            height: 40,
            child: TextField(
              decoration: InputDecoration(hintText: "search addresses"),
              controller: addressSearchController,
              onChanged: (newQuery) {
                addressQuery = newQuery;
                _searchAddresses();
                setState(() {
                  addressQueryOffset =
                      addressSearchController.selection.extentOffset;
                  if (addressQueryOffset < 0) {
                    addressQueryOffset = 0;
                  }
                });
              },
            )),
        Container(
            width: 350,
            height: 260,
            child: Scrollbar(
                child: ListView.builder(
                    padding: EdgeInsets.all(4.0),
                    itemCount: addressChoices.length,
                    itemBuilder: (BuildContext context, int index) {
                      String address = addressChoices[index];
                      bool checked = finalAddressValue.contains(address);
                      return ListTile(
                          onTap: () {
                            setState(() {
                              finalAddressValue =
                                  address == finalAddressValue ? "" : address;
                            });
                          },
                          title: Row(children: [
                            checked ? chosenIcon : notChosenIcon,
                            Text(address)
                          ]));
                    }))),

        Text(""), //temporary spacer
        Header("Program"),
        DropdownButton<String>(
          value: programsValue,
          iconEnabledColor: Colors.black,
          items: programs_dropdown,
          hint: Text("Select a program"),
          onChanged: (newValue) {
            setState(() {
              finalProgramValue = newValue;
              programsValue = newValue;
            });
          },
        ),
      ],
    );
    TextEditingController otherIssuesController =
        TextEditingController(text: otherIssues);
    otherIssuesController.selection =
        TextSelection.collapsed(offset: otherIssuesOffset);
    Icon checkedIcon = Icon(Icons.check_box, color: Colors.green);
    Icon notCheckedIcon = Icon(Icons.check_box_outline_blank);
    Widget page2 = Column(children: [
      Padding(
          padding: EdgeInsets.fromLTRB(0, 15, 0, 0), child: Header("Issues")),
      Expanded(
          child: Scrollbar(
              child: ListView.builder(
                  padding: EdgeInsets.all(4.0),
                  itemCount: issues.length,
                  itemBuilder: (BuildContext context, int index) {
                    String issue = issues[index];
                    bool checked = issuesChecked.contains(issue);
                    return ListTile(
                        onTap: () {
                          setState(() {
                            if (checked) {
                              issuesChecked.remove(issue);
                            } else {
                              issuesChecked.add(issue);
                            }
                          });
                        },
                        title: Row(children: [
                          checked ? checkedIcon : notCheckedIcon,
                          Text(issue)
                        ]));
                  }))),
      Padding(
          padding: EdgeInsets.fromLTRB(0, 20, 0, 20),
          child: Container(
              width: 300,
              height: 60,
              child: TextField(
                controller: otherIssuesController,
                onChanged: (newStr) {
                  otherIssues = newStr;
                  otherIssuesOffset =
                      otherIssuesController.selection.extentOffset;
                  if (otherIssuesOffset < 0) {
                    otherIssuesOffset = 0;
                  }
                },
                decoration:
                    new InputDecoration(hintText: "List any other issues here"),
              )))
      // TextField(controller: TextEditingController(text: "other issue"),),

      // TextField(controller: TextEditingController(text: "other response")),
    ]);
    TextEditingController otherResponsesController =
        TextEditingController(text: otherResponses);
    otherResponsesController.selection =
        TextSelection.collapsed(offset: otherResponsesOffset);
    Widget page3 = Column(
      children: <Widget>[
        Padding(
            padding: EdgeInsets.fromLTRB(0, 15, 0, 0),
            child: Header("Responses")),
        Expanded(
            child: Scrollbar(
                child: ListView.builder(
                    padding: EdgeInsets.all(4.0),
                    itemCount: responses.length,
                    itemBuilder: (BuildContext context, int index) {
                      String response = responses[index];
                      bool checked = responsesChecked.contains(response);
                      return ListTile(
                          onTap: () {
                            setState(() {
                              if (checked) {
                                responsesChecked.remove(response);
                              } else {
                                responsesChecked.add(response);
                              }
                            });
                          },
                          title: Row(children: [
                            checked ? checkedIcon : notCheckedIcon,
                            Text(response)
                          ]));
                    }))),
        Padding(
            padding: EdgeInsets.fromLTRB(0, 20, 0, 20),
            child: Container(
                width: 300,
                height: 60,
                child: TextField(
                  controller: otherResponsesController,
                  onChanged: (newStr) {
                    otherResponses = newStr;
                    otherResponsesOffset =
                        otherResponsesController.selection.extentOffset;
                    if (otherResponsesOffset < 0) {
                      otherResponsesOffset = 0;
                    }
                  },
                  decoration: new InputDecoration(
                      hintText: "List any other responses here"),
                )))
      ],
    );
    Widget page4 = Column(
      children: <Widget>[
        Padding(
            padding: EdgeInsets.fromLTRB(0, 15, 0, 0), child: Header("Status")),
        DropdownButton<String>(
          value: status,
          iconEnabledColor: Colors.black,
          items: _getStatusOptions(),
          hint: Text("Choose a status"),
          onChanged: (newValue) {
            setState(() {
              status = newValue;
            });
          },
        ),
        Header("Comments"),
        Container(
            width: 300,
            height: 60,
            child: TextField(
              controller: textController,
              onChanged: (newText) {
                comments = newText;
                commentsOffset = textController.selection.extentOffset;
                if (commentsOffset < 0) {
                  commentsOffset = 0;
                }
              },
            )),
      ],
    );
    List<String> sortedIssues = issuesChecked.toList();
    if (otherIssues != "") {
      sortedIssues.add(otherIssues);
    }
    sortedIssues.sort();
    List<String> sortedResponses = responsesChecked.toList();
    if (otherResponses != "") {
      sortedResponses.add(otherResponses);
    }
    sortedResponses.sort();
    List<List<String>> categories = [
      ["Address", finalAddressValue],
      ["Program", finalProgramValue],
      ["Issues", sortedIssues.join(", ")],
      ["Responses", sortedResponses.join(", ")],
      ["Status", status],
      ["Comments", comments]
    ];
    List<Widget> subPage = categories.map((category) {
      return Column(
        children: <Widget>[
          Align(
              alignment: Alignment.centerLeft,
              child: Padding(
                  padding: EdgeInsets.fromLTRB(15, 0, 0, 0),
                  child: Header(category[0]))),
          Align(
              alignment: Alignment.centerLeft,
              child: Padding(
                  padding: EdgeInsets.fromLTRB(25, 10, 0, 0),
                  child: Text(category[1]))),
          Divider(),
        ],
      );
    }).toList();
    Widget header = Column(children: <Widget>[Header("Review and Submit")]);
    subPage.insert(0, header);
    Widget finalPage = Padding(
        padding: EdgeInsets.fromLTRB(0, 15, 0, 0),
        child: Scrollbar(
            child: ListView(
          children: subPage,
        )));

    List<Widget> pages = [page1, page2, page3, page4, finalPage];
    numPages = pages.length;
    return ListView(children: [
      Container(
          height: MediaQuery.of(context).size.height / 1.45,
          width: MediaQuery.of(context).size.width,
          child: pages[toShow]),
      _getNavButtons()
    ]);
  }
}

class EncounterForm extends StatefulWidget {
  @override
  EncounterFormState createState() => EncounterFormState();
}
