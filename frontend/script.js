document.getElementById('add-process').addEventListener('click', () => {
    const processDiv = document.createElement('div');
    processDiv.classList.add('process');
    processDiv.innerHTML = `
        <label>Arrival Time: <input type="number" name="arrivalTime"></label>
        <label>Burst Time: <input type="number" name="burstTime"></label>
        <label>Priority: <input type="number" name="priority"></label>
    `;
    document.getElementById('processes').appendChild(processDiv);
});

document.getElementById('scheduler-form').addEventListener('submit', async (event) => {
    event.preventDefault();

    const processes = Array.from(document.querySelectorAll('.process')).map(processDiv => {
        const arrivalTime = processDiv.querySelector('input[name="arrivalTime"]').value;
        const burstTime = processDiv.querySelector('input[name="burstTime"]').value;
        const priority = processDiv.querySelector('input[name="priority"]').value;
        return { arrivalTime, burstTime, priority };
    });

    const quantumTime = document.querySelector('input[name="quantumTime"]').value;

    const response = await fetch('/run-scheduler', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ processes, quantumTime })
    });

    const result = await response.text();
    document.getElementById('output').innerText = result;
});
