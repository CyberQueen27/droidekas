const form = document.querySelector('form');

form.addEventListener('submit', (event) => {
    event.preventDefault();
    const website = event.target.elements.website.value;
    const username = event.target.elements.username.value;
    const password = event.target.elements.password.value;

    addPassword(website, username, password);
    const websitePasswords = getPassword(website);
    console.log(websitePasswords);
    
    const secretKey = 'MY_SECRET_KEY';
    const totp = generateTOTP(secretKey);
    console.log(totp);
  });

function addPassword(website, username, password) {
    const passwords = JSON.parse(localStorage.getItem('passwords')) || {};
    
    if (!passwords[website]) {
      passwords[website] = [];
    }
    
    passwords[website].push({ username, password });
    
    localStorage.setItem('passwords', JSON.stringify(passwords));
}
  
// Get password function
function getPassword(website) {
    const passwords = JSON.parse(localStorage.getItem('passwords')) || {};
    
    const websitePasswords = passwords[website];
    
    if (!websitePasswords) {
      return [];
    }
    
    return websitePasswords.map(({ username, password }) => ({ username, password }));
}

// Generate TOTP function
function generateTOTP(secretKey) {
    const otp = require('otp-auth');
    const totp = otp({ secret: secretKey });
    
    return totp.generate();
}

// Generate Password
function generatePassword() {
  // Password length
  const length = 10;

  // Character set
  const charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+~`|}{[]\:;?><,./-=";

  let password = "";
  for (let i = 0, n = charset.length; i < length; ++i) {
    password += charset.charAt(Math.floor(Math.random() * n));
  }

  // Display password
  document.getElementById("password").value = password;
}
  
