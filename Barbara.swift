import SwiftUI         // For building UI components
import Alamofire       // For networking with OpenAI API
import AVFoundation    // For audio playback and recording
import Speech          // For speech recognition
import FirebaseAuth    // For authentication with Firebase
import FirebaseCore    // For initializing Firebase
import StoreKit        // For handling in-app purchases

// MARK: - AppDelegate
// Entry point of the app where Firebase is configured
@main  // Indicates the application's entry point
class AppDelegate: UIResponder, UIApplicationDelegate {
    // Function called when the app has finished launching
    func application(_ application: UIApplication,
        didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        FirebaseApp.configure()  // Initialize Firebase
        return true  // Indicate successful launch
    }
    // ... (other app delegate methods can be added here)
}

// MARK: - OpenAIAPI
// Class responsible for communicating with the OpenAI API
class OpenAIAPI {
    static let shared = OpenAIAPI()  // Singleton instance for global access
    
    // Securely store your API key; do not hard-code it in the code
    private let apiKey = ProcessInfo.processInfo.environment["OPENAI_API_KEY"] ?? ""
    
    // Function to send a message to the OpenAI API and receive a response
    func sendMessage(_ message: String, completion: @escaping (String) -> Void) {
        let url = "https://api.openai.com/v1/chat/completions"  // OpenAI API endpoint
        let headers: HTTPHeaders = [  // HTTP headers for the request
            "Authorization": "Bearer \(apiKey)",  // Authorization header with the API key
            "Content-Type": "application/json"  // Content type header
        ]
        let parameters: [String: Any] = [  // Parameters for the request body
            "model": "gpt-3.5-turbo",  // Specify the model to use
            "messages": [  // Messages array containing the conversation
                ["role": "user", "content": message]  // The user's message
            ]
        ]
        
        // Make the POST request using Alamofire
        AF.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default, headers: headers)
            .responseDecodable(of: OpenAIResponse.self) { response in  // Decode the response into OpenAIResponse
                switch response.result {
                case .success(let value):
                    // Extract the assistant's reply from the response
                    if let content = value.choices.first?.message.content {
                        completion(content.trimmingCharacters(in: .whitespacesAndNewlines))  // Return the content
                    } else {
                        completion("I'm sorry, I didn't understand that.")  // Handle empty content
                    }
                case .failure(let error):
                    print("Error: \(error.localizedDescription)")  // Print the error
                    completion("I'm sorry, there was an error processing your request.")  // Inform the user of the error
                }
            }
    }
}

// Define the response model structs conforming to Decodable
struct OpenAIResponse: Decodable {
    let choices: [Choice]  // Array of choices in the response
}

struct Choice: Decodable {
    let message: MessageContent  // The message content
}

struct MessageContent: Decodable {
    let role: String  // The role (e.g., "assistant")
    let content: String  // The message content
}

// MARK: - Message Model
// Struct representing a chat message
struct Message: Identifiable {
    let id = UUID()  // Unique identifier for each message
    let text: String  // The message text content
    let isUser: Bool  // Boolean indicating if the message is from the user
}

// MARK: - Views

// MARK: LoadingView
// Initial loading screen with the app logo and name
struct LoadingView: View {
    var body: some View {
        VStack {  // Vertical stack to arrange elements vertically
            Spacer()  // Push content to the center by adding space above
            Image("BarbaraLogo")  // Display the logo image named "BarbaraLogo"
                .resizable()  // Make the image resizable
                .aspectRatio(contentMode: .fit)  // Maintain aspect ratio within the frame
                .frame(width: 200, height: 200)  // Set the size of the image
            Text("Barbara")  // Display the app name
                .font(.largeTitle)  // Set the font size to large title
                .foregroundColor(Color("PrimaryColor"))  // Set the text color to the primary color defined in assets
                .padding()  // Add padding around the text
            Spacer()  // Push content to the center by adding space below
        }
    }
}

// MARK: ChatView
// Main chat interface where users can send and receive messages
struct ChatView: View {
    @State private var messageText = ""  // State variable to hold the current text input
    @State private var messages: [Message] = [  // State variable to hold all messages
        // Initialize with a greeting message from Barbara
        Message(text: "Hello! How can I assist you today?", isUser: false)
    ]
    @State private var showMenu = false  // State variable to control the display of the side menu
    @State private var showMembershipPrompt = false  // State to control membership prompt
    @State private var messageCount = 0  // Counter for the number of messages sent
    
    var body: some View {
        NavigationView {  // Embed the view in a navigation view for a navigation bar
            VStack {  // Vertical stack for arranging elements vertically
                ScrollView {  // Scrollable view to display messages
                    ForEach(messages) { message in  // Iterate over each message
                        MessageBubble(message: message)  // Display each message in a bubble
                    }
                }
                .padding(.top)  // Add padding at the top
                
                HStack {  // Horizontal stack for the input field and send button
                    TextField("Type a message...", text: $messageText)  // Text input field bound to messageText
                        .padding()  // Add padding inside the text field
                        .background(Color(.systemGray6))  // Set background color
                        .cornerRadius(20)  // Round the corners
                        .disableAutocorrection(true)  // Disable autocorrection
                        .font(.system(size: 18))  // Set the font size
                        
                    Button(action: sendMessage) {  // Send button with action
                        HStack {  // Horizontal stack for icon and text
                            Image(systemName: "paperplane.fill")  // Send icon
                            Text("Send")  // Button label
                                .fontWeight(.bold)  // Bold text
                        }
                        .padding()  // Add padding around the button content
                        .background(Color.blue)  // Set background color
                        .foregroundColor(.white)  // Set text color
                        .cornerRadius(20)  // Round the corners
                    }
                }
                .padding()  // Add padding around the HStack
            }
            .navigationBarTitle("Barbara", displayMode: .inline)  // Set the navigation bar title
            .navigationBarItems(leading: Button(action: {
                showMenu.toggle()  // Toggle the display of the side menu
            }) {
                Image(systemName: "line.horizontal.3")  // Hamburger menu icon
                    .imageScale(.large)  // Set the icon size
            })
            .sheet(isPresented: $showMenu) {  // Present the side menu as a sheet
                SideMenuView()  // The side menu view
            }
            .onAppear {
                // Reset message count on view appear
                messageCount = 0
            }
            .alert(isPresented: $showMembershipPrompt) {
                // Alert to prompt the user to become a member
                Alert(
                    title: Text("Membership Required"),
                    message: Text("You've reached your free limit. Become a member to continue using Barbara."),
                    primaryButton: .default(Text("Become a Member"), action: {
                        // Navigate to MembershipView or handle purchase
                    }),
                    secondaryButton: .cancel()
                )
            }
        }
    }
    
    // Function to handle sending messages
    func sendMessage() {
        let userMessage = Message(text: messageText, isUser: true)  // Create a message object for the user's message
        messages.append(userMessage)  // Add the user's message to the messages array
        let input = messageText  // Store the input text
        messageText = ""  // Clear the input field
        
        messageCount += 1  // Increment the message count
        if messageCount > 5 {  // Check if the free limit is reached (e.g., 5 messages)
            showMembershipPrompt = true  // Show the membership prompt
            return  // Stop processing further
        }
        
        // Call the OpenAI API to get a response
        OpenAIAPI.shared.sendMessage(input) { response in
            DispatchQueue.main.async {  // Ensure UI updates happen on the main thread
                let botMessage = Message(text: response, isUser: false)  // Create a message object for Barbara's response
                messages.append(botMessage)  // Add Barbara's response to the messages array
            }
        }
    }
}

// MARK: MessageBubble
// View representing a single message bubble in the chat
struct MessageBubble: View {
    var message: Message  // The message to display
    
    var body: some View {
        HStack {
            if message.isUser {
                Spacer()  // Push content to the right
                Text(message.text)  // Display user's message text
                    .padding()  // Add padding around the text
                    .background(Color.blue)  // Set background color for user's message
                    .foregroundColor(.white)  // Set text color
                    .cornerRadius(20)  // Round the corners
                    .frame(maxWidth: 250, alignment: .trailing)  // Set max width and align right
            } else {
                Text(message.text)  // Display Barbara's message text
                    .padding()  // Add padding
                    .background(Color(.systemGray5))  // Set background color for Barbara's message
                    .cornerRadius(20)  // Round the corners
                    .frame(maxWidth: 250, alignment: .leading)  // Set max width and align left
                Spacer()  // Push content to the left
            }
        }
        .padding(message.isUser ? .trailing : .leading)  // Add padding on the appropriate side
        .padding(.vertical, 5)  // Add vertical padding
    }
}

// MARK: VoiceAssistantView
// View for voice assistant functionality with speech recognition and text-to-speech
struct VoiceAssistantView: View {
    @State private var isListening = false  // State variable to track if the app is listening
    @State private var recognizedText = ""  // State variable to hold recognized speech text
    @State private var assistantResponse = ""  // State variable for assistant's response
    @State private var isProcessing = false  // State variable to indicate processing state
    
    private let speechRecognizer = SFSpeechRecognizer()  // Initialize the speech recognizer
    private let audioEngine = AVAudioEngine()  // Audio engine for recording audio
    private var recognitionRequest: SFSpeechAudioBufferRecognitionRequest?  // Recognition request object
    private var recognitionTask: SFSpeechRecognitionTask?  // Recognition task object
    private let synthesizer = AVSpeechSynthesizer()  // Synthesizer for text-to-speech
    
    var body: some View {
        VStack {
            Spacer()  // Push content to the center
            if isProcessing {
                // Show a progress view when processing
                ProgressView("Thinking...")
                    .progressViewStyle(CircularProgressViewStyle())
                    .padding()
            } else if !assistantResponse.isEmpty {
                // Display the assistant's response
                Text(assistantResponse)
                    .font(.title)
                    .padding()
            } else {
                // Prompt the user to start speaking
                Text("Tap the button and start speaking")
                    .font(.title)
                    .padding()
            }
            Spacer()
            // Button to start or stop listening
            Button(action: {
                isListening ? stopListening() : startListening()  // Toggle listening state
            }) {
                Image(systemName: isListening ? "mic.fill" : "mic.slash.fill")  // Microphone icon
                    .resizable()
                    .frame(width: 100, height: 100)
                    .padding()
            }
            .foregroundColor(isListening ? .red : .blue)  // Change color based on listening state
            .accessibility(label: Text(isListening ? "Stop Listening" : "Start Listening"))  // Accessibility label
        }
        .onDisappear {
            if isListening {
                stopListening()  // Stop listening when the view disappears
            }
        }
    }
    
    // Function to start listening and recognizing speech
    func startListening() {
        SFSpeechRecognizer.requestAuthorization { authStatus in  // Request authorization for speech recognition
            if authStatus == .authorized {
                do {
                    try startRecording()  // Start recording audio
                    isListening = true  // Update listening state
                } catch {
                    print("Error starting recording: \(error.localizedDescription)")
                }
            } else {
                print("Speech recognition authorization denied.")
            }
        }
    }
    
    // Function to stop listening
    func stopListening() {
        audioEngine.stop()  // Stop the audio engine
        recognitionRequest?.endAudio()  // End the recognition request
        isListening = false  // Update listening state
    }
    
    // Function to start recording audio and recognizing speech
    func startRecording() throws {
        if recognitionTask != nil {
            recognitionTask?.cancel()  // Cancel any existing recognition task
            recognitionTask = nil  // Reset the recognition task
        }
        
        let audioSession = AVAudioSession.sharedInstance()  // Get the shared audio session
        try audioSession.setCategory(.record, mode: .measurement, options: .duckOthers)  // Configure the audio session
        try audioSession.setActive(true, options: .notifyOthersOnDeactivation)  // Activate the audio session
        
        recognitionRequest = SFSpeechAudioBufferRecognitionRequest()  // Create a new recognition request
        
        let inputNode = audioEngine.inputNode  // Get the audio engine's input node
        guard let recognitionRequest = recognitionRequest else {
            fatalError("Unable to create recognition request.")
        }
        
        recognitionRequest.shouldReportPartialResults = true  // Report partial results
        
        // Start the recognition task
        recognitionTask = speechRecognizer?.recognitionTask(with: recognitionRequest) { result, error in
            if let result = result {
                recognizedText = result.bestTranscription.formattedString  // Update recognized text
            }
            
            if error != nil || (result?.isFinal ?? false) {
                audioEngine.stop()  // Stop the audio engine
                inputNode.removeTap(onBus: 0)  // Remove the tap on the audio bus
                recognitionRequest.endAudio()  // End the recognition request
                isListening = false  // Update listening state
                processRecognizedText()  // Process the recognized text
            }
        }
        
        let recordingFormat = inputNode.outputFormat(forBus: 0)  // Get the audio format
        // Install a tap on the audio input node to capture audio samples
        inputNode.installTap(onBus: 0, bufferSize: 1024, format: recordingFormat) { buffer, when in
            recognitionRequest?.append(buffer)  // Append the audio buffer to the recognition request
        }
        
        audioEngine.prepare()  // Prepare the audio engine
        try audioEngine.start()  // Start the audio engine
    }
    
    // Function to process the recognized speech text
    func processRecognizedText() {
        isProcessing = true  // Update processing state
        // Send the recognized text to the OpenAI API
        OpenAIAPI.shared.sendMessage(recognizedText) { response in
            DispatchQueue.main.async {  // Ensure UI updates happen on the main thread
                self.assistantResponse = response  // Update the assistant's response
                self.isProcessing = false  // Update processing state
                self.speak(text: response)  // Speak out the assistant's response
            }
        }
    }
    
    // Function to perform text-to-speech
    func speak(text: String) {
        let utterance = AVSpeechUtterance(string: text)  // Create a speech utterance with the text
        utterance.voice = AVSpeechSynthesisVoice(language: "en-US")  // Set the voice language
        synthesizer.speak(utterance)  // Start speaking the utterance
    }
}

// MARK: SideMenuView
// Side menu providing navigation options and account management
struct SideMenuView: View {
    @Environment(\.presentationMode) var presentationMode  // Environment variable to control presentation
    @State private var showProfile = false  // State variable to control profile view
    @State private var showLogin = false  // State variable to control login view
    @State private var isLoggedIn = Auth.auth().currentUser != nil  // Check if user is logged in
    
    var body: some View {
        NavigationView {
            List {
                // Navigation link to start a new message
                NavigationLink(destination: ChatView()) {
                    Text("Start New Message")
                }
                // Button to show profile or prompt login
                Button(action: {
                    if isLoggedIn {
                        showProfile = true  // Show profile if logged in
                    } else {
                        showLogin = true  // Show login if not logged in
                    }
                }) {
                    Text("My Profile")
                }
                // Button to delete the account
                if isLoggedIn {
                    Button(action: deleteAccount) {
                        Text("Delete Account")
                            .foregroundColor(.red)  // Highlight the delete action in red
                    }
                }
            }
            .navigationBarTitle("Menu", displayMode: .inline)
            .navigationBarItems(trailing: Button("Close") {
                presentationMode.wrappedValue.dismiss()  // Close the side menu
            })
            .sheet(isPresented: $showProfile) {
                ProfileView()  // Present the profile view
            }
            .sheet(isPresented: $showLogin) {
                LoginView()  // Present the login view
            }
        }
    }
    
    // Function to handle account deletion
    func deleteAccount() {
        // Implement account deletion logic using Firebase Auth
        let user = Auth.auth().currentUser
        user?.delete { error in
            if let error = error {
                print("Error deleting account: \(error.localizedDescription)")
            } else {
                // Handle successful deletion, e.g., navigate to login screen
                isLoggedIn = false  // Update login state
            }
        }
    }
}

// MARK: ProfileView
// Profile view displaying user information
struct ProfileView: View {
    @Environment(\.presentationMode) var presentationMode  // Environment variable to control presentation
    @State private var userName = "User Name"  // Placeholder for user's name
    @State private var phoneNumber = "Phone Number"  // Placeholder for phone number
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Profile")) {
                    // Display user's name (replace with actual data retrieval)
                    Text("Name: \(userName)")
                    // Display user's phone number (replace with actual data retrieval)
                    Text("Phone: \(phoneNumber)")
                }
            }
            .navigationTitle("My Profile")  // Set the navigation title
            .navigationBarItems(trailing: Button("Close") {
                presentationMode.wrappedValue.dismiss()  // Close the profile view
            })
            .onAppear {
                // Fetch user data from Firebase
                if let user = Auth.auth().currentUser {
                    phoneNumber = user.phoneNumber ?? "Unknown"
                    // Fetch additional user data if stored
                }
            }
        }
    }
}

// MARK: LoginView
// View for user authentication using phone number verification
struct LoginView: View {
    @Environment(\.presentationMode) var presentationMode  // Environment variable to control presentation
    @State private var phoneNumber = ""  // State variable for the phone number input
    @State private var verificationCode = ""  // State variable for the verification code input
    @State private var verificationID: String?  // Store the verification ID received from Firebase
    @State private var isVerificationSent = false  // State to track if verification code is sent
    
    var body: some View {
        NavigationView {
            VStack {
                if !isVerificationSent {
                    // Phone number input field
                    TextField("Enter your phone number", text: $phoneNumber)
                        .keyboardType(.phonePad)  // Set keyboard type to phone pad
                        .padding()  // Add padding
                        .background(Color(.systemGray6))  // Set background color
                        .cornerRadius(10)  // Round the corners
                        .padding()
                    // Button to send the verification code
                    Button(action: sendCode) {
                        Text("Send Verification Code")
                            .padding()
                            .background(Color.blue)
                            .foregroundColor(.white)
                            .cornerRadius(10)
                    }
                } else {
                    // Verification code input field
                    TextField("Enter verification code", text: $verificationCode)
                        .keyboardType(.numberPad)  // Set keyboard type to number pad
                        .padding()
                        .background(Color(.systemGray6))  // Set background color
                        .cornerRadius(10)
                        .padding()
                    // Button to verify the code
                    Button(action: verifyCode) {
                        Text("Verify Code")
                            .padding()
                            .background(Color.green)
                            .foregroundColor(.white)
                            .cornerRadius(10)
                    }
                }
            }
            .navigationBarTitle("Login", displayMode: .inline)
            .navigationBarItems(trailing: Button("Close") {
                presentationMode.wrappedValue.dismiss()  // Close the login view
            })
        }
    }
    
    // Function to send the verification code to the user's phone
    func sendCode() {
        // Use Firebase to send the verification code
        PhoneAuthProvider.provider().verifyPhoneNumber(phoneNumber, uiDelegate: nil) { verificationID, error in
            if let error = error {
                print("Error sending code: \(error.localizedDescription)")  // Print any errors
                return
            }
            self.verificationID = verificationID  // Store the verification ID
            self.isVerificationSent = true  // Update state to show verification code input
        }
    }
    
    // Function to verify the code entered by the user
    func verifyCode() {
        guard let verificationID = verificationID else { return }  // Ensure verification ID is available
        // Create a credential using the verification ID and code
        let credential = PhoneAuthProvider.provider().credential(
            withVerificationID: verificationID,
            verificationCode: verificationCode
        )
        // Sign in with the credential
        Auth.auth().signIn(with: credential) { authResult, error in
            if let error = error {
                print("Error verifying code: \(error.localizedDescription)")  // Print any errors
                return
            }
            // Dismiss the login view upon successful sign-in
            presentationMode.wrappedValue.dismiss()
        }
    }
}

// MARK: MembershipView
// View to prompt the user to become a member after reaching free usage limit
struct MembershipView: View {
    var body: some View {
        VStack {
            Text("You've reached your free limit.")  // Inform the user
                .font(.title)
                .padding()
            Text("Become a member to continue using Barbara.")  // Prompt for membership
                .padding()
            // Button to initiate purchase
            Button(action: purchaseMembership) {
                Text("Become a Member")
                    .padding()
                    .background(Color.blue)
                    .foregroundColor(.white)
                    .cornerRadius(10)
            }
        }
    }
    
    // Function to handle membership purchase
    func purchaseMembership() {
        // Implement in-app purchase logic here
        // This involves using StoreKit to initiate the purchase process
    }
}

// MARK: - App Entry Point
// The main content view that decides which view to display
struct ContentView: View {
    @State private var isLoading = true  // State variable to control loading screen
    @State private var showLogin = false  // State variable to control login view
    
    var body: some View {
        if isLoading {
            LoadingView()  // Show the loading view
                .onAppear {
                    // Simulate loading delay and then proceed
                    DispatchQueue.main.asyncAfter(deadline: .now() + 2) {
                        isLoading = false  // Update loading state
                        if Auth.auth().currentUser == nil {
                            showLogin = true  // Show login if user is not authenticated
                        }
                    }
                }
        } else {
            ChatView()  // Show the main chat view
                .fullScreenCover(isPresented: $showLogin) {
                    LoginView()  // Present the login view
                }
        }
    }
}

// MARK: - Preview Provider
// Preview for ContentView (optional)
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
