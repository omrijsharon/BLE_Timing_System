let pilotNames = {};  // Store names of pilots
let winners = {};  // Store winners of each heat
let tournament;
let totalPilots;
let winsCount = {};

window.onload = function() {

    document.getElementById('play-button').addEventListener('click', function () {
        var audio = document.getElementById('ready-set-go-audio');
        audio.play();
    });
    // Get the modal and close button elements
    let modal = document.getElementById('standings-modal');
    let closeButton = document.querySelector('.close');

    // When the user clicks the standings button, open the modal
    document.getElementById('standings-button').onclick = function() {
        updateStandings();
        modal.style.display = 'block';
    };

    // When the user clicks the close button, close the modal
    closeButton.onclick = function() {
        modal.style.display = 'none';
    };

    // When the user clicks anywhere outside of the modal, close it
    window.onclick = function(event) {
        let modal = document.getElementById('standings-modal');
        if (event.target == modal) {
            modal.style.display = "none";
        }
    }

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
    document.getElementById('save-button').addEventListener('click', function() {
        if (!tournament || tournament.length === 0) {
            alert('No tournament to save!');
            return;
        }
        
        saveTournament(tournament, winners, pilotNames, totalPilots);
    });

    document.getElementById('load-button').addEventListener('click', function(event) {
        event.preventDefault();
        
        // Create a 'change' event listener for the file input
        document.getElementById('load-file').addEventListener('change', function() {
            if (this.files && this.files[0]) {
                loadTournament(this.files[0]);
                this.value = null; // clear the selected file
            }
        });
    
        // Simulate a click on the file input
        document.getElementById('load-file').click();
    });
    // Add event listener for form submission
    document.getElementById('tournament-form').addEventListener('submit', function(event) {
        // Prevent the form from submitting normally
        event.preventDefault();

        // Reset data from previous tournament
        pilotNames = {};
        winners = {};
        winsCount = {};

        // Get input values and convert them to integers
        totalPilots = parseInt(document.getElementById('total-pilots').value, 10);
        let pilotsPerHeat = parseInt(document.getElementById('pilots-per-heat').value, 10);

        // Validate inputs
        if (totalPilots <= 0 || pilotsPerHeat <= 0) {
            alert('Invalid input. Total pilots and pilots per heat must be greater than zero.');
            return;
        }

        // Initialize pilot names
        for (let i = 1; i <= totalPilots; i++) {
            pilotNames[i] = "Pilot " + i;
        }

        // Generate tournament
        tournament = roundRobin(totalPilots, pilotsPerHeat);

        // Test tournament
        let matrix = testRoundRobin(tournament, totalPilots, pilotsPerHeat);

        // Output tournament
        renderTable(tournament, totalPilots, pilotNames, winners);
    }); 
};

function calculateStandings() {
    winsCount = {};  // Object to store the count of wins for each pilot

    // Initialize the wins count for each pilot
    for (let pilot in pilotNames) {
        winsCount[pilotNames[pilot]] = 0;
    }

    // Count the wins for each pilot
    for (let roundHeat in winners) {
        let pilot = winners[roundHeat];
        if (winsCount[pilot] !== undefined) {
            winsCount[pilot]++;
        }
    }

    // Create an array of [pilot, wins] pairs and sort it in descending order by wins
    let standings = Object.entries(winsCount).sort((a, b) => b[1] - a[1]);

    return standings;
}

function markWinner(roundHeat) {
    let selectElement = document.getElementById('winner-' + roundHeat);
    let winner = selectElement.options[selectElement.selectedIndex].text;
    winners[winner] = (winners[winner] || 0) + 1; // Increase the count of wins for the winning pilot
}

function calculateStandings() {
    let standings = Object.entries(winsCount).sort((a, b) => b[1] - a[1]);
    return standings;
}

function updateStandings() {
    let standings = calculateStandings();
    let standingsList = document.getElementById('standings-list');
    standingsList.innerHTML = '';

    for (let i = 0; i < standings.length; i++) {
        let pilot = standings[i][0];
        let wins = standings[i][1];

        // Don't add the competitor to the list if they have 0 wins
        if (wins > 0) {
            standingsList.innerHTML += pilot + ': ' + wins + ' wins</p>';
        }
    }

    // Show the modal
    let modal = document.getElementById('standings-modal');
    modal.style.display = "flex";
}

function selectWinner(event) {
    event.preventDefault();
    let round = parseInt(this.getAttribute("data-round"), 10);
    let heat = parseInt(this.getAttribute("data-heat"), 10);
    let pilot = parseInt(this.getAttribute("data-pilot"), 10);

    // Save the previous winner for this round-heat pair
    let previousWinner = winners[`${round}-${heat}`];

    // If there was a previous winner, decrease their wins count
    if (previousWinner !== undefined) {
        winsCount[pilotNames[previousWinner]]--;
    }

    // Update the winner for this round-heat pair
    winners[`${round}-${heat}`] = pilot;

    // Increase the wins count for the new winning pilot
    let winnerName = pilotNames[pilot];
    winsCount[winnerName] = (winsCount[winnerName] || 0) + 1;

    renderTable(tournament, totalPilots, pilotNames, winners);
}

function clickPilot(roundNumber, heatNumber, pilot) {
    winners[`${roundNumber}-${heatNumber}`] = pilot;
    updateTable(tournament);
}

function updateTable(tournament) {
    let table = "<table>";
    for (let roundNumber = 0; roundNumber < tournament.length; roundNumber++) {
        table += `<tr><th colspan="${tournament[roundNumber][0].length + 2}">Round ${roundNumber+1}</th></tr>`;
        for (let heatNumber = 0; heatNumber < tournament[roundNumber].length; heatNumber++) {
            table += `<tr class="heat-row"><td>Heat ${heatNumber+1}</td>`;
            table += tournament[roundNumber][heatNumber].map(pilot => `<td><span class="pilot" onclick="clickPilot(${roundNumber}, ${heatNumber}, ${pilot})">Pilot ${pilot}</span></td>`).join("");
            table += `<td>${winners[`${roundNumber}-${heatNumber}`] ? 'Pilot ' + winners[`${roundNumber}-${heatNumber}`] : ''}</td></tr>`;
        }
    }
    table += "</table>";
    document.getElementById('tournament-output').innerHTML = table;
}

function renderTable(tournament, totalPilots, pilotNames, winners) {
    let table = "<table>";
    for (let roundNumber = 0; roundNumber < tournament.length; roundNumber++) {
        table += `<tr><th colspan="${tournament[roundNumber][0].length + 1}">Round ${roundNumber+1}</th><th>Winner</th></tr>`;
        for (let heatNumber = 0; heatNumber < tournament[roundNumber].length; heatNumber++) {
            table += `<tr class="heat-row"><td>Heat ${heatNumber+1}</td>`;
            table += tournament[roundNumber][heatNumber].map(pilot => {
                if (pilot > totalPilots) { // If pilot is a dummy
                    return `<td></td>`; // Return an empty cell
                }
                let pilotName = pilotNames[pilot];
                let link = `<a href="#" class="pilot-link" data-round="${roundNumber}" data-heat="${heatNumber}" data-pilot="${pilot}">${pilotName}</a>`;
                return `<td>${link}</td>`;
            }).join("");
            table += `<td class="winner-column">${winners[`${roundNumber}-${heatNumber}`] ? pilotNames[winners[`${roundNumber}-${heatNumber}`]] : ''}</td>`;
            table += "</tr>";
        }
    }
    table += "</table>";
    document.getElementById('tournament-output').innerHTML = table;

    let pilotLinks = document.getElementsByClassName("pilot-link");
    for (let i = 0; i < pilotLinks.length; i++) {
        pilotLinks[i].addEventListener('click', selectWinner);
        pilotLinks[i].addEventListener('contextmenu', function(event) {
            event.preventDefault();
            let pilot = parseInt(this.getAttribute("data-pilot"), 10);
            let newName = prompt('Enter new name for ' + pilotNames[pilot], pilotNames[pilot]);
            if (newName !== null && newName !== '') {
                pilotNames[pilot] = newName;
                renderTable(tournament, totalPilots, pilotNames, winners);
            }
        });
    }
}

function saveTournament(tournament, winners, pilotNames, totalPilots) {
    if (!tournament || tournament.length === 0) {
        alert('No tournament to save!');
        return;
    }

    let saveData = {
        tournament: tournament,
        winners: winners,
        pilotNames: pilotNames,
        totalPilots: totalPilots  // added this line
    };

    let blob = new Blob([JSON.stringify(saveData)], { type: "application/json" });
    let url = URL.createObjectURL(blob);

    let a = document.createElement('a');
    a.href = url;
    a.download = 'tournament_' + new Date().toISOString().split('T')[0] + '.json';
    a.click();
}

function loadTournament(file) {
    let reader = new FileReader();

    reader.onload = function(e) {
        try {
            let loadedData = JSON.parse(e.target.result);

            // Check if the loaded data has necessary properties
            if (!loadedData.tournament || !loadedData.winners || !loadedData.pilotNames || !loadedData.totalPilots) {
                throw new Error('Invalid file structure.');
            }

            // If everything is okay, assign the loaded data
            tournament = loadedData.tournament;
            winners = loadedData.winners;
            pilotNames = loadedData.pilotNames;
            totalPilots = loadedData.totalPilots;  // added this line

            // Re-render the table
            renderTable(tournament, totalPilots, pilotNames, winners);
        } catch (error) {
            // In case of an error during loading or parsing the file, display an alert
            alert('Failed to load the tournament: ' + error.message);
        }
    };

    reader.onerror = function() {
        // In case of a FileReader error, display an alert
        alert('Failed to read the file.');
    };

    // Start reading the file
    reader.readAsText(file);
}
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