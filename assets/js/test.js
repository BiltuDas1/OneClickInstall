// Data (Same as before)
const appsData = [
    { id: 1, name: "Google Chrome", category: "browsers", desc: "Fast and secure web browser built by Google.", icon: "fa-brands fa-chrome", color: "#ea4335" },
    { id: 2, name: "Firefox", category: "browsers", desc: "Free and open-source web browser by Mozilla.", icon: "fa-brands fa-firefox", color: "#ff9400" },
    { id: 3, name: "Microsoft Edge", category: "browsers", desc: "Fast and secure browser by Microsoft.", icon: "fa-brands fa-edge", color: "#0078d7" },
    { id: 4, name: "VS Code", category: "development", desc: "Code editing. Redefined.", icon: "fa-solid fa-code", color: "#22a5f1" },
    { id: 5, name: "Git", category: "development", desc: "Version control system.", icon: "fa-brands fa-git-alt", color: "#f05032" },
    { id: 6, name: "Node.js", category: "development", desc: "JavaScript runtime.", icon: "fa-brands fa-node", color: "#68a063" },
    { id: 7, name: "VLC Player", category: "multimedia", desc: "The best open source multimedia player.", icon: "fa-solid fa-file-video", color: "#ff8800" },
    { id: 8, name: "Spotify", category: "multimedia", desc: "Digital music service.", icon: "fa-brands fa-spotify", color: "#1db954" },
    { id: 9, name: "Zoom", category: "utilities", desc: "Video conferencing tool.", icon: "fa-solid fa-video", color: "#2d8cff" },
    { id: 10, name: "WinRAR", category: "utilities", desc: "Archive manager.", icon: "fa-solid fa-box-archive", color: "#7b52ab" }
];

let selectedApps = new Set();
let currentCategory = 'all';

// --- Initialization ---
updateSidebarCounts();
renderApps();

// --- 1. Welcome Screen Logic (Updated for Exit Animation) ---
function closeWelcome() {
    const screen = document.getElementById('welcomeScreen');
    
    // 1. Add 'exit' class to trigger CSS scaleOut animation
    screen.classList.add('exit'); 
    
    // 2. Hide element completely after animation finishes (500ms = 0.5s)
    setTimeout(() => {
        screen.style.display = 'none';
    }, 500); 
}

// --- 2. Modal Logic (How to Use) ---
function openGuide() {
    const modal = document.getElementById('guideModal');
    modal.style.display = 'flex';
}

function closeGuide() {
    document.getElementById('guideModal').style.display = 'none';
}

// Close modal if clicked outside the box
window.onclick = function(event) {
    const modal = document.getElementById('guideModal');
    if (event.target == modal) {
        modal.style.display = "none";
    }
}

// --- 3. Core App Functions (Same as before) ---
function filterCategory(category, element) {
    currentCategory = category;
    document.querySelectorAll('.sidebar li').forEach(li => li.classList.remove('active'));
    element.classList.add('active');
    renderApps();
}

function renderApps() {
    const appContainer = document.getElementById('appContainer');
    appContainer.innerHTML = '';

    const filteredApps = currentCategory === 'all' 
        ? appsData 
        : appsData.filter(app => app.category === currentCategory);

    filteredApps.forEach(app => {
        const isSelected = selectedApps.has(app.id);
        const card = document.createElement('div');
        card.className = `app-card ${isSelected ? 'selected' : ''}`;
        card.onclick = () => toggleSelection(app.id);

        card.innerHTML = `
            <div class="card-top">
                <div class="app-icon" style="color: ${app.color}"><i class="${app.icon}"></i></div>
                <div class="app-details"><h3>${app.name}</h3><p>${app.desc}</p></div>
            </div>
            <div class="card-bottom">
                <div class="select-text">${isSelected ? '<i class="fa-solid fa-check"></i> Selected' : 'Click to select'}</div>
                <button class="add-btn">${isSelected ? 'Added' : '+ Add'}</button>
            </div>
        `;
        appContainer.appendChild(card);
    });
}

function toggleSelection(id) {
    if (selectedApps.has(id)) selectedApps.delete(id);
    else selectedApps.add(id);
    updateFooter();
    renderApps();
}

function updateFooter() {
    const count = selectedApps.size;
    document.getElementById('selectedCount').innerText = count;
    document.getElementById('btnCount').innerText = count;
    
    const btn = document.getElementById('installBtn');
    if(count > 0) {
        btn.style.opacity = "1";
        btn.style.cursor = "pointer";
    } else {
        btn.style.opacity = "0.7";
    }
}

function updateSidebarCounts() {
    document.getElementById('count-all').innerText = appsData.length;
    const countCat = (cat) => appsData.filter(a => a.category === cat).length;
    document.getElementById('count-browsers').innerText = countCat('browsers');
    document.getElementById('count-development').innerText = countCat('development');
    document.getElementById('count-multimedia').innerText = countCat('multimedia');
    document.getElementById('count-utilities').innerText = countCat('utilities');
}

function searchApps() {
    const query = document.getElementById('searchInput').value.toLowerCase();
    const appContainer = document.getElementById('appContainer');
    appContainer.innerHTML = '';
    
    const filteredApps = appsData.filter(app => app.name.toLowerCase().includes(query));
    
    filteredApps.forEach(app => {
        const isSelected = selectedApps.has(app.id);
        const card = document.createElement('div');
        card.className = `app-card ${isSelected ? 'selected' : ''}`;
        card.onclick = () => toggleSelection(app.id);
        
        card.innerHTML = `
            <div class="card-top">
                <div class="app-icon" style="color: ${app.color}"><i class="${app.icon}"></i></div>
                <div class="app-details"><h3>${app.name}</h3><p>${app.desc}</p></div>
            </div>
            <div class="card-bottom">
                <div class="select-text">${isSelected ? 'Selected' : 'Select'}</div>
                <button class="add-btn">${isSelected ? 'Added' : '+ Add'}</button>
            </div>
        `;
        appContainer.appendChild(card);
    });
}