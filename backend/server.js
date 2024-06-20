const express = require('express');
const bodyParser = require('body-parser');
const { execFile } = require('child_process');
const path = require('path');

const app = express();
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, '../frontend')));

app.post('/run-scheduler', (req, res) => {
    const { processes, quantumTime } = req.body;

    // Prepare the input for the C++ program
    let input = `${processes.length}\n`;
    processes.forEach(p => {
        input += `${p.arrivalTime} ${p.burstTime} ${p.priority}\n`;
    });
    input += `${quantumTime}\n`;

    // Execute the compiled C++ program
    const schedulerPath = path.join(__dirname, 'scheduler');
    const child = execFile(schedulerPath, [], (error, stdout, stderr) => {
        if (error) {
            console.error(`Error executing scheduler: ${error}`);
            return res.status(500).send('Error executing scheduler');
        }
        res.send(stdout);
    });

    // Provide the input to the C++ program
    child.stdin.write(input);
    child.stdin.end();
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});
