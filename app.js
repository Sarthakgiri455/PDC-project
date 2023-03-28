const express = require('express');
const { spawn } = require('child_process');
const bodyParser = require('body-parser');
const fs = require('fs');

const app = express();
app.set('view engine', 'ejs');


app.use(bodyParser.urlencoded({
  extended: true
}));

app.get('/', (req, res) => {
  const summary = "helloworld";
  res.render('index', { summary });
});

app.post('/', (req, res) => {
  const text = req.body.text;
  const fileName = 'input.txt';

  fs.writeFileSync(fileName, text, (err) => {
    if (err) throw err;

    const cProgramPath = './tfidf';
    const cProgramProcess = spawn(cProgramPath);

    let output = '';

    cProgramProcess.stdout.on('data', (data) => {
      output += data.toString();
    });

    cProgramProcess.stderr.on('data', (data) => {
      console.error(data.toString());
    });

    cProgramProcess.on('close', (code) => {
      console.log(`child process exited with code ${code}`);

      const summaryFileName = 'output.txt';
      const summary = fs.readFileSync(summaryFileName, 'utf-8');

      res.render('index', { summary });
    });
  });
});

app.listen(3000, () => {
  console.log('Server started on port 3000');
});