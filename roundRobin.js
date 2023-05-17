window.onload = function() {
    // Generate options for total pilots dropdown
    const totalPilotsSelect = document.getElementById('total-pilots');
    for (let i = 5; i <= 50; i++) {
        const option = document.createElement('option');
        option.value = i;
        option.text = i;
        if (i === 12) { // Set default selection
            option.selected = true;
        }
        totalPilotsSelect.appendChild(option);
    }

    // Generate options for pilots per heat dropdown
    const pilotsPerHeatSelect = document.getElementById('pilots-per-heat');
    for (let i = 2; i <= 8; i++) {
        const option = document.createElement('option');
        option.value = i;
        option.text = i;
        if (i === 4) { // Set default selection
            option.selected = true;
        }
        pilotsPerHeatSelect.appendChild(option);
    }

    // Add event listener for form submission
    document.getElementById('tournament-form').addEventListener('submit', function(event) {
        // Prevent the form from submitting normally
        event.preventDefault();

        // Get input values and convert them to integers
        let totalPilots = parseInt(document.getElementById('total-pilots').value, 10);
        let pilotsPerHeat = parseInt(document.getElementById('pilots-per-heat').value, 10);

        // Validate inputs
        if (totalPilots <= 0 || pilotsPerHeat <= 0) {
            alert('Invalid input. Total pilots and pilots per heat must be greater than zero.');
            return;
        }

        // Generate tournament
        let tournament = roundRobin(totalPilots, pilotsPerHeat);
        // Test tournament
        let matrix = testRoundRobin(tournament, totalPilots, pilotsPerHeat);

        // Convert the matrix to a string
        let matrixString = matrix.map(row => row.join(', ')).join('\n');

        // Display the matrix in an alert dialog
        alert(matrixString);
        // Output tournament
        let formattedTournament = "";
        for (let roundNumber = 0; roundNumber < tournament.length; roundNumber++) {
            formattedTournament += `Round ${roundNumber+1}:\n`;
            for (let heatNumber = 0; heatNumber < tournament[roundNumber].length; heatNumber++) {
                formattedTournament += `Heat ${heatNumber+1}: ${tournament[roundNumber][heatNumber].join(', ')}\n`;
            }
            formattedTournament += "\n";
        }
        let table = "<table>";
        for (let roundNumber = 0; roundNumber < tournament.length; roundNumber++) {
            table += `<tr><th colspan="${tournament[roundNumber][0].length + 1}">Round ${roundNumber+1}</th></tr>`;
            for (let heatNumber = 0; heatNumber < tournament[roundNumber].length; heatNumber++) {
                table += `<tr class="heat-row"><td>Heat ${heatNumber+1}</td>`;
                table += tournament[roundNumber][heatNumber].map(pilot => `<td>Pilot ${pilot}</td>`).join("");
                table += "</tr>";
            }
        }
        table += "</table>";
        document.getElementById('tournament-output').innerHTML = table;
    });    
};

// --- the rest of the roundRobin and testRoundRobin functions...

function roll(array, shift) {
    let len = array.length;
    let newArray = new Array(len);

    for (let i = 0; i < len; i++) {
        newArray[(i + shift + len) % len] = array[i];
    }

    return newArray;
}

function diag(matrix, k = 0) {
    let arr = [];
    let row, col;

    if (k >= 0) {
        row = 0;
        col = k;
    } else {
        row = -k;
        col = 0;
    }

    while (row < matrix.length && col < matrix[row].length) {
        arr.push(matrix[row][col]);
        row++;
        col++;
    }

    return k < 0 ? arr.reverse() : arr;
}

function roundRobin(n, k) {
    // Check that n is a multiple of k
    if (n % k !== 0) { // add dummy pilots to make n a multiple of k
        console.log(`Adding ${k - (n % k)} dummy pilots to make n a multiple of k`);
        n += k - (n % k);
    }

    // Construct initial round with pilots grouped sequentially
    let rounds = [Array.from({length: n}, (_, i) => i).reduce((a, v, i) => {
        if (i % k === 0) a.push([]);
        a[a.length - 1].push(v + 1);
        return a;
    }, [])];

    // Generate subsequent rounds by rotating pilots, excluding the first one
    for (let round_number = 1; round_number < n - 1; round_number++) {
        let previous_round = rounds[rounds.length - 1];
        let diags = roll(Array.from({length: k}, (_, i) => i).concat(
            Array.from({length: previous_round.length - 1}, (_, i) => -i - 1)
        ), (n + 1) * round_number);
        let new_round = diags.map(i => diag(previous_round, i)).reduce((a, v) => {
            while (v.length) {
                if (!a.length || a[a.length - 1].length === k) a.push([]);
                a[a.length - 1].push(v.shift());
            }
            return a;
        }, []);
        rounds.push(new_round);
    }

    return rounds;
}

// Helper function to divide an array into chunk of size n
Array.prototype.chunk = function (n) {
    let res = [];
    for(let i = 0; i < this.length; i += n) {
        res.push(this.slice(i, i + n));
    }
    return res;
};



function testRoundRobin(tournament, n, k) {
    // Check that n is a multiple of k and adjust if necessary
    if (n % k !== 0) {
        n += k - (n % k);
    }

    // Create an n x n matrix initialized to 0
    let matrix = Array.from({length: n}, () => Array(n).fill(0));

    // Iterate over the tournament
    for (let round of tournament) {
        for (let heat of round) {
            for (let i = 0; i < heat.length; i++) {
                for (let j = i+1; j < heat.length; j++) {
                    // Increment the count for each pair of pilots
                    matrix[heat[i]-1][heat[j]-1] += 1;
                    matrix[heat[j]-1][heat[i]-1] += 1;
                }
            }
        }
    }

    // Fill diagonal with 0s
    for (let i = 0; i < n; i++) {
        matrix[i][i] = 0;
    }

    return matrix;
}