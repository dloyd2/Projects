using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class ScoreManager : MonoBehaviour
{
    int totalScore = 0;
    static int scoreToAdd = 0;
    [SerializeField] TextMeshProUGUI scoreUI;
    
    public static void ChangeScore(int changeInScore)
    {
        scoreToAdd += changeInScore;
    }
    void UpdateScore()
    {
        totalScore += scoreToAdd;
        Debug.Log("score: " + totalScore);
        scoreUI.text = totalScore.ToString();
        scoreToAdd = 0;
    }
    
    void Awake()
    {
        scoreUI.text = totalScore.ToString();
    }
    void Start()
    {
        totalScore = 0;
        scoreToAdd = 0;
    }
    
    // Update is called once per frame
    void Update()
    {
        if(scoreToAdd != 0)
        {
            UpdateScore();
        }
    }
}
